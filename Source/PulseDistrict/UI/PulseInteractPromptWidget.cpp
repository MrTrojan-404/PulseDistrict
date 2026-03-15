#include "PulseInteractPromptWidget.h"
#include "Components/TextBlock.h"

void UPulseInteractPromptWidget::SetPromptVisibility(bool bShow, const FString& TargetName)
{
	if (bShow)
	{
		if (PromptText)
		{
			PromptText->SetText(FText::FromString(FString::Printf(TEXT("Press [E] to view %s's Profile"), *TargetName)));
		}
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}