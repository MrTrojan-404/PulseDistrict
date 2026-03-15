// Copyright NexusMultiplayer. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interfaces/INexusProfileInterface.h"

class UNakamaClient;
class UNakamaSession;
class UNexusNakamaProfileProxy; // Proxy for UObject dynamic delegates
struct FNakamaStoreObjectAcks;
struct FNakamaStorageObjectList;
struct FNakamaError;

class NEXUSMULTIPLAYER_API FNexusNakamaProfile : public INexusProfileInterface
{
public:
	FNexusNakamaProfile(UNakamaClient* InClient, UNakamaSession* InSession);
	~FNexusNakamaProfile();

	virtual void SaveProfile(const FString& StorageKey, const FNexusUserProfile& Profile) override;
	virtual void FetchProfile(const FString& UserId) override;

	// Called by subsystem when session refreshes
	void UpdateSession(UNakamaSession* NewSession);

	// Storage Callbacks
	void HandleSaveSuccess(const FNakamaStoreObjectAcks& Acks);
	void HandleSaveError(const FNakamaError& Error);
	void HandleFetchSuccess(const FNakamaStorageObjectList& Objects);
	void HandleFetchError(const FNakamaError& Error);

private:
	UNakamaClient* Client = nullptr;
	UNakamaSession* Session = nullptr;

	bool EnsureClientAndSession(const FString& CallerName) const;

	TObjectPtr<UNexusNakamaProfileProxy> Proxy;
};