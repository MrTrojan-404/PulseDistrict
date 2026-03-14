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
	// If we are currently in the main menu editing our own profile, ignore this event here.
    
	if (!bSuccess)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (AgeText)
	{
		AgeText->SetText(FText::AsNumber(Profile.Age));
	}

	if (AboutMeText)
	{
		AboutMeText->SetText(FText::FromString(Profile.AboutMe));
	}

	// Unhide the widget to show the data!
	SetVisibility(ESlateVisibility::Visible);
}

void UPulseProfileHoverWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}