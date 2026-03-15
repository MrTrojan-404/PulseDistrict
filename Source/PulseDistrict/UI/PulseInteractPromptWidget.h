#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PulseInteractPromptWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class PULSEDISTRICT_API UPulseInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Call this to show/hide the prompt
	void SetPromptVisibility(bool bShow, const FString& TargetName = TEXT(""));

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PromptText;
};