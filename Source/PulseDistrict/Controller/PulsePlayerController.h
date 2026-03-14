#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PulsePlayerController.generated.h"

// Delegate for the UMG Chat Widget to listen to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatMessageReceived, const FString&, SenderName, const FString&, Message);

UCLASS()
class PULSEDISTRICT_API APulsePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Call this from your Input Action (e.g., Left Click or 'E' key)
	UFUNCTION(BlueprintCallable, Category = "Pulse|Interaction")
	void InspectPlayerProfile();

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

protected:
	// Server RPC: The client asks the server to distribute the message
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(const FString& Message);

	// Client RPC: The server tells a specific client to display a message
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FString& SenderName, const FString& Message);
};