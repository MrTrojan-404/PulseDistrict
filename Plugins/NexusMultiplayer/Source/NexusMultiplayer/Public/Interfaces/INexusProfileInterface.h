// Copyright NexusMultiplayer. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

struct FNexusUserProfile;

class NEXUSMULTIPLAYER_API INexusProfileInterface
{
public:
	virtual ~INexusProfileInterface() = default;

	virtual void SaveProfile(const FNexusUserProfile& Profile) = 0;
	virtual void FetchProfile(const FString& UserId) = 0;

	TFunction<void(bool /*bSuccess*/)> OnProfileSaved;
	TFunction<void(bool /*bSuccess*/, const FNexusUserProfile& /*Profile*/)> OnProfileFetched;
};
