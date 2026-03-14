#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/NexusTypes.h"
#include "PulseProfileHoverWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS(Abstract)
class PULSEDISTRICT_API UPulseProfileHoverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AgeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AboutMeText;

	// Optional: A button to close the hover card
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

private:
	// Listens to the Subsystem for the Nakama JSON reply
	UFUNCTION()
	void HandleProfileFetchedRemote(bool bSuccess, FNexusUserProfile Profile);

	UFUNCTION()
	void HandleCloseClicked();
};