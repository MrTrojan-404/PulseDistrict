#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PulseChatWidget.generated.h"

class UEditableTextBox;
class UScrollBox;

UCLASS(Abstract)
class PULSEDISTRICT_API UPulseChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// The box that displays the history of messages
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatHistoryScrollBox;

	// The box where the player types their message
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

private:
	// Listens to the Player Controller for incoming messages
	UFUNCTION()
	void HandleChatMessageReceived(const FString& SenderName, const FString& Message);

	// Listens to the Input Box for when the player presses Enter
	UFUNCTION()
	void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};