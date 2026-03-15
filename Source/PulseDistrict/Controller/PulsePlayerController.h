#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PulsePlayerController.generated.h"

// Delegate for the UMG Chat Widget to listen to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatMessageReceived, const FString&, SenderName, const FString&, Message);

class UInputAction;

UCLASS()
class PULSEDISTRICT_API APulsePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	
	// The max distance the player can be clicked from
	UPROPERTY(EditDefaultsOnly, Category = "Pulse|Interaction")
	float InspectRange = 1500.0f;

	// ── Chat API ──────────────────────────────────────────────────────────────

	// UMG calls this when the player hits 'Enter'
	UFUNCTION(BlueprintCallable, Category = "Pulse|Chat")
	void SendChatMessage(const FString& Message);

	// Fires locally when a message arrives from the server
	UPROPERTY(BlueprintAssignable, Category = "Pulse|Chat")
	FOnChatMessageReceived OnChatMessageReceived;

	UFUNCTION(BlueprintCallable, Category = "Pulse|Input")
	void OpenChat();

	UFUNCTION(BlueprintCallable, Category = "Pulse|Chat")
	void CloseChat();
	
protected:
	virtual void SetupInputComponent() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pulse|Input")
	TObjectPtr<UInputAction> InspectAction;

	UPROPERTY(EditDefaultsOnly, Category = "Pulse|Input")
	TObjectPtr<UInputAction> TextChatAction;
	
	// Server RPC: The client asks the server to distribute the message
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(const FString& Message);

	// Client RPC: The server tells a specific client to display a message
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FString& SenderName, const FString& Message);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Pulse|UI")
	TSubclassOf<class UPulseMasterHUDWidget> MasterHUDClass;

	UPROPERTY()
	TObjectPtr<UPulseMasterHUDWidget> MasterHUDInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Pulse|UI")
	TSubclassOf<class UPulseInteractPromptWidget> InteractPromptClass;

	UPROPERTY()
	TObjectPtr<UPulseInteractPromptWidget> InteractPromptInstance;
private:
	void InspectPlayerProfile();

	void CheckForInteractTarget();
};