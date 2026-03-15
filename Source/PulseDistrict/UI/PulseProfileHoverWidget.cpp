#include "PulseProfileHoverWidget.h"
#include "Core/NexusMultiplayerSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPulseProfileHoverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind to the subsystem's fetch delegate
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNexusMultiplayerSubsystem* Sub = GI->GetSubsystem<UNexusMultiplayerSubsystem>())
		{
			Sub->OnProfileFetched.AddDynamic(this, &UPulseProfileHoverWidget::HandleProfileFetchedRemote);
		}
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UPulseProfileHoverWidget::HandleCloseClicked);
	}

	// Hide the widget by default until we click on someone
	SetVisibility(ESlateVisibility::Hidden);
}

void UPulseProfileHoverWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNexusMultiplayerSubsystem* Sub = GI->GetSubsystem<UNexusMultiplayerSubsystem>())
		{
			Sub->OnProfileFetched.RemoveDynamic(this, &UPulseProfileHoverWidget::HandleProfileFetchedRemote);
		}
	}
	Super::NativeDestruct();
}

void UPulseProfileHoverWidget::HandleProfileFetchedRemote(bool bSuccess, FNexusUserProfile Profile)
{
	if (!bSuccess)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (AgeText) AgeText->SetText(FText::AsNumber(Profile.Age));
	if (AboutMeText) AboutMeText->SetText(FText::FromString(Profile.AboutMe));

	SetVisibility(ESlateVisibility::Visible);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}

void UPulseProfileHoverWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}
void UPulseProfileHoverWidget::SetTargetName(const FString& InName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(InName));
	}
}