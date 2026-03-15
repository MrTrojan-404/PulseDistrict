#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PulseMasterHUDWidget.generated.h"

class UPulseChatWidget;
class UPulseProfileHoverWidget;
class UPulseInteractPromptWidget;

UCLASS(Abstract)
class PULSEDISTRICT_API UPulseMasterHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Expose these so the PlayerController can call their functions
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPulseChatWidget> ChatWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPulseProfileHoverWidget> ProfileHoverWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPulseInteractPromptWidget> InteractPromptWidget;
};