// Copyright NexusMultiplayer. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NakamaError.h"
#include "NakamaStorageObject.h"
#include "NexusNakamaProfileProxy.generated.h"

class FNexusNakamaProfile;

UCLASS()
class NEXUSMULTIPLAYER_API UNexusNakamaProfileProxy : public UObject
{
	GENERATED_BODY()

public:
	// Raw pointer is safe here because the owner (FNexusNakamaProfile) manages this object's lifecycle
	FNexusNakamaProfile* Owner = nullptr;

	UFUNCTION() void OnSaveSuccess(const FNakamaStoreObjectAcks& Acks);
	UFUNCTION() void OnSaveError(const FNakamaError& Error);

	UFUNCTION() void OnFetchSuccess(const FNakamaStorageObjectList& Objects);
	UFUNCTION() void OnFetchError(const FNakamaError& Error);
};