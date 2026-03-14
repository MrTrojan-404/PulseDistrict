// Copyright NexusMultiplayer. All Rights Reserved.

#include "Backend/Nakama/Profile/NexusNakamaProfileProxy.h"
#include "Backend/Nakama/Profile/NexusNakamaProfile.h"

void UNexusNakamaProfileProxy::OnSaveSuccess(const FNakamaStoreObjectAcks& Acks)
{
	if (Owner) Owner->HandleSaveSuccess(Acks);
}

void UNexusNakamaProfileProxy::OnSaveError(const FNakamaError& Error)
{
	if (Owner) Owner->HandleSaveError(Error);
}

void UNexusNakamaProfileProxy::OnFetchSuccess(const FNakamaStorageObjectList& Objects)
{
	if (Owner) Owner->HandleFetchSuccess(Objects);
}

void UNexusNakamaProfileProxy::OnFetchError(const FNakamaError& Error)
{
	if (Owner) Owner->HandleFetchError(Error);
}