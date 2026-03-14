#include "PulsePlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Core/NexusMultiplayerSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APulsePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Cast to the Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind the Inspect Action to our Raycast function
		if (InspectAction)
		{
			EnhancedInput->BindAction(InspectAction, ETriggerEvent::Started, this, &APulsePlayerController::InspectPlayerProfile);
		}
	}
}

void APulsePlayerController::InspectPlayerProfile()
{
	// 1.Get screen center
	
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	FVector WorldLocation, WorldDirection;
	if (!DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return;
	}

	// 2. Set up the Line Trace
	FVector EndLocation = WorldLocation + (WorldDirection * InspectRange);
	FHitResult HitResult;
    
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn()); // Don't hit ourselves

	// 3. Fire the Trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, EndLocation, ECC_Visibility, Params))
	{
		// 4. Did we hit a character?
		if (ACharacter* HitCharacter = Cast<ACharacter>(HitResult.GetActor()))
		{
			// 5. Extract their Unique Net ID (Nakama ID)
			if (APlayerState* PS = HitCharacter->GetPlayerState())
			{
				if (PS->GetUniqueId().IsValid())
				{
					FString TargetUserId = PS->GetUniqueId()->ToString();
                    
					// 6.Subsystem -> fetch the data
					if (UNexusMultiplayerSubsystem* Sub = GetGameInstance()->GetSubsystem<UNexusMultiplayerSubsystem>())
					{
						// UI should already be listening to Sub->OnProfileFetched to pop up the Player card
						Sub->FetchUserProfile(TargetUserId);
					}
				}
			}
		}
	}
}

// ── Chat Implementation ───────────────────────────────────────────────────────

void APulsePlayerController::SendChatMessage(const FString& Message)
{
	// Don't send empty messages
	if (Message.TrimStartAndEnd().IsEmpty()) return;

	// Route to the server
	Server_SendChatMessage(Message);
}

// Validation prevents malicious clients from crashing the server
bool APulsePlayerController::Server_SendChatMessage_Validate(const FString& Message)
{
	// Example: Block messages longer than 200 characters
	return Message.Len() <= 200; 
}

void APulsePlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	// 1. Figure out who sent it. APlayerState automatically replicates the DisplayName
	// we set earlier via Nakama/Steam!
	FString SenderName = TEXT("Unknown Dancer");
	if (PlayerState)
	{
		SenderName = PlayerState->GetPlayerName();
	}

	// Sanitize the message just to be safe
	FString SafeMessage = Message.Left(200);

	// 2. Iterate through all connected players in the Disco map
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APulsePlayerController* PC = Cast<APulsePlayerController>(It->Get()))
		{
			// 3. Tell their client to receive the message
			PC->Client_ReceiveChatMessage(SenderName, SafeMessage);
		}
	}
}

void APulsePlayerController::Client_ReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	// 4. We are now back on the local client. Broadcast the delegate so UMG can update the UI.
	OnChatMessageReceived.Broadcast(SenderName, Message);
}

