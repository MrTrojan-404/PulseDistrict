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

public:
	// Call this from the Player Controller right before asking the database for the rest
	void SetTargetName(const FString& InName);

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
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NameText;

private:
	// Listens to the Subsystem for the Nakama JSON reply
	UFUNCTION()
	void HandleProfileFetchedRemote(bool bSuccess, FNexusUserProfile Profile);

	UFUNCTION()
	void HandleCloseClicked();
};