#include "PulsePlayerController.h"
#include "GameFramework/Pawn.h" // Changed to Pawn
#include "GameFramework/PlayerState.h"
#include "Core/NexusMultiplayerSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/EditableTextBox.h"
#include "PulseDistrict/UI/PulseChatWidget.h"
#include "PulseDistrict/UI/PulseInteractPromptWidget.h"
#include "PulseDistrict/UI/PulseProfileHoverWidget.h"
#include "PulseDistrict/UI/HUD/PulseMasterHUDWidget.h"

void APulsePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
       if (MasterHUDClass)
       {
          MasterHUDInstance = CreateWidget<UPulseMasterHUDWidget>(this, MasterHUDClass);
          if (MasterHUDInstance)
          {
             MasterHUDInstance->AddToViewport();
                
             if (MasterHUDInstance->InteractPromptWidget)
             {
                MasterHUDInstance->InteractPromptWidget->SetPromptVisibility(false);
             }
          }
       }
    }
}

void APulsePlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (IsLocalPlayerController())
    {
       CheckForInteractTarget();
    }
}

void APulsePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
       // Changed to ETriggerEvent::Started so it only fires once per key press!
       if (InspectAction)
       {
          EnhancedInput->BindAction(InspectAction, ETriggerEvent::Started, this, &APulsePlayerController::InspectPlayerProfile);
       }
       if (TextChatAction)
       {
          EnhancedInput->BindAction(TextChatAction, ETriggerEvent::Started, this, &APulsePlayerController::OpenChat);
       }
    }
}

void APulsePlayerController::CheckForInteractTarget()
{
    // Fixed: Properly accessing through MasterHUDInstance
    if (!MasterHUDInstance || !MasterHUDInstance->InteractPromptWidget) return;

    int32 ViewportSizeX, ViewportSizeY;
    GetViewportSize(ViewportSizeX, ViewportSizeY);
    FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

    FVector WorldLocation, WorldDirection;
    if (!DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection)) return;

    FVector EndLocation = WorldLocation + (WorldDirection * InspectRange);
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn()); 

    if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, EndLocation, ECC_Visibility, Params))
    {
       // Fixed: Cast to APawn instead of ACharacter
       if (APawn* HitPawn = Cast<APawn>(HitResult.GetActor()))
       {
          if (APlayerState* PS = HitPawn->GetPlayerState())
          {
             MasterHUDInstance->InteractPromptWidget->SetPromptVisibility(true, PS->GetPlayerName());
             return;
          }
       }
    }

    MasterHUDInstance->InteractPromptWidget->SetPromptVisibility(false);
}

void APulsePlayerController::OpenChat()
{
	if (MasterHUDInstance && MasterHUDInstance->ChatWidget && MasterHUDInstance->ChatWidget->ChatInputBox)
	{
		FInputModeGameAndUI InputMode;
       
		// Focus directly on the text box, not the parent widget!
		InputMode.SetWidgetToFocus(MasterHUDInstance->ChatWidget->ChatInputBox->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		SetShowMouseCursor(true);

		// Call focus AFTER setting the input mode
		MasterHUDInstance->ChatWidget->FocusChatInput();
	}
}

void APulsePlayerController::CloseChat()
{
    // Returns full control to the player's movement
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    SetShowMouseCursor(false);
}

void APulsePlayerController::InspectPlayerProfile()
{
    int32 ViewportSizeX, ViewportSizeY;
    GetViewportSize(ViewportSizeX, ViewportSizeY);
    FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

    FVector WorldLocation, WorldDirection;
    if (!DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection)) return;

    FVector EndLocation = WorldLocation + (WorldDirection * InspectRange);
    FHitResult HitResult;
    
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn()); 

   if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, EndLocation, ECC_Visibility, Params))
       {
          if (APawn* HitPawn = Cast<APawn>(HitResult.GetActor()))
          {
             if (APlayerState* PS = HitPawn->GetPlayerState())
             {
                // Fallback for PIE testing: Use the real ID if valid, otherwise use the machine ID
                FString TargetUserId;
                if (PS->GetUniqueId().IsValid())
                {
                    TargetUserId = PS->GetUniqueId()->ToString();
                }
                else
                {
                    TargetUserId = FPlatformMisc::GetLoginId();
                }
   
                // 1. Tell the UI widget who we just looked at
                if (MasterHUDInstance && MasterHUDInstance->ProfileHoverWidget)
                {
                    MasterHUDInstance->ProfileHoverWidget->SetTargetName(PS->GetPlayerName());
                }
   
                // 2. Tell the Subsystem to fetch their data!
                if (UNexusMultiplayerSubsystem* Sub = GetGameInstance()->GetSubsystem<UNexusMultiplayerSubsystem>())
                {
                    Sub->FetchUserProfile(TargetUserId);
                }
             }
          }
       }
}

// ── Chat Implementation ───────────────────────────────────────────────────────

void APulsePlayerController::SendChatMessage(const FString& Message)
{
    if (Message.TrimStartAndEnd().IsEmpty()) return;
    Server_SendChatMessage(Message);
}

bool APulsePlayerController::Server_SendChatMessage_Validate(const FString& Message)
{
    return Message.Len() <= 200; 
}

void APulsePlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
    FString SenderName = TEXT("Unknown Dancer");
    if (PlayerState)
    {
       SenderName = PlayerState->GetPlayerName();
    }

    FString SafeMessage = Message.Left(200);

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
       if (APulsePlayerController* PC = Cast<APulsePlayerController>(It->Get()))
       {
          PC->Client_ReceiveChatMessage(SenderName, SafeMessage);
       }
    }
}

void APulsePlayerController::Client_ReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
    OnChatMessageReceived.Broadcast(SenderName, Message);
}