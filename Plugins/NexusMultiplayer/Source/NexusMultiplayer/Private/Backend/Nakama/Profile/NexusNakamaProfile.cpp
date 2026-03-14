#include "Backend/Nakama/Profile/NexusNakamaProfile.h"
#include "NakamaClient.h"
#include "NakamaSession.h"
#include "NakamaStorageObject.h"
#include "Backend/Nakama/Profile/NexusNakamaProfileProxy.h"
#include "Core/NexusLog.h"
#include "Core/NexusTypes.h"

FNexusNakamaProfile::FNexusNakamaProfile(UNakamaClient* InClient, UNakamaSession* InSession)
    : Client(InClient), Session(InSession)
{
    Proxy = NewObject<UNexusNakamaProfileProxy>(GetTransientPackage());
    Proxy->Owner = this;
    Proxy->AddToRoot();
}

FNexusNakamaProfile::~FNexusNakamaProfile()
{
    if (Proxy)
    {
        Proxy->Owner = nullptr;
        Proxy->RemoveFromRoot();
        Proxy = nullptr;
    }
}

void FNexusNakamaProfile::UpdateSession(UNakamaSession* NewSession)
{
    Session = NewSession;
}

void FNexusNakamaProfile::SaveProfile(const FNexusUserProfile& Profile)
{
    if (!EnsureClientAndSession(TEXT("SaveProfile"))) return;

    FString SafeAbout = Profile.AboutMe.Replace(TEXT("\""), TEXT("\\\""));
    FString Payload = FString::Printf(TEXT("{\"age\":%d, \"about\":\"%s\"}"), Profile.Age, *SafeAbout);

    FNakamaStoreObjectWrite WriteObj;
    WriteObj.Collection = TEXT("user_profiles");
    WriteObj.Key = Session->SessionData.UserId;
    WriteObj.Value = Payload;
    WriteObj.PermissionRead = ENakamaStoragePermissionRead::PUBLIC_READ;
    WriteObj.PermissionWrite = ENakamaStoragePermissionWrite::OWNER_WRITE;

    FOnStorageObjectAcks Success;
    Success.AddDynamic(Proxy, &UNexusNakamaProfileProxy::OnSaveSuccess);
    FOnError Error;
    Error.AddDynamic(Proxy, &UNexusNakamaProfileProxy::OnSaveError);

    Client->WriteStorageObjects(Session, {WriteObj}, Success, Error);
}

void FNexusNakamaProfile::FetchProfile(const FString& UserId)
{
    if (!EnsureClientAndSession(TEXT("FetchProfile"))) return;

    FNakamaReadStorageObjectId ReadId;
    ReadId.Collection = TEXT("user_profiles");
    ReadId.Key = UserId;
    ReadId.UserId = UserId;

    FOnStorageObjectsRead Success;
    Success.AddDynamic(Proxy, &UNexusNakamaProfileProxy::OnFetchSuccess);
    FOnError Error;
    Error.AddDynamic(Proxy, &UNexusNakamaProfileProxy::OnFetchError);

    Client->ReadStorageObjects(Session, {ReadId}, Success, Error);
}

void FNexusNakamaProfile::HandleSaveSuccess(const FNakamaStoreObjectAcks& Acks)
{
    NEXUS_SUCCESS("[NakamaProfile] Profile saved successfully");
    if (OnProfileSaved) OnProfileSaved(true);
}

void FNexusNakamaProfile::HandleSaveError(const FNakamaError& Error)
{
    NEXUS_ERROR("[NakamaProfile] SaveProfile failed: %s", *Error.Message);
    if (OnProfileSaved) OnProfileSaved(false);
}

void FNexusNakamaProfile::HandleFetchSuccess(const FNakamaStorageObjectList& Objects)
{
    FNexusUserProfile Profile;

    if (Objects.Objects.Num() > 0)
    {
        const FString& Json = Objects.Objects[0].Value;
        NEXUS_LOG("[NakamaProfile] Fetched payload: %s", *Json);

        // Basic string parsing to extract JSON fields
        int32 AgeIdx = Json.Find(TEXT("\"age\":"));
        if (AgeIdx != INDEX_NONE) 
        {
            int32 CommaIdx = Json.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, AgeIdx);
            FString AgeStr = Json.Mid(AgeIdx + 6, CommaIdx - (AgeIdx + 6)).TrimStartAndEnd();
            Profile.Age = FCString::Atoi(*AgeStr);
        }

        int32 AboutIdx = Json.Find(TEXT("\"about\":\""));
        if (AboutIdx != INDEX_NONE) 
        {
            int32 EndQuote = Json.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, AboutIdx + 9);
            Profile.AboutMe = Json.Mid(AboutIdx + 9, EndQuote - (AboutIdx + 9));
        }
    }
    else
    {
        NEXUS_WARN("[NakamaProfile] No profile object found for user");
    }

    if (OnProfileFetched) OnProfileFetched(true, Profile);
}

void FNexusNakamaProfile::HandleFetchError(const FNakamaError& Error)
{
    NEXUS_ERROR("[NakamaProfile] FetchProfile failed: %s", *Error.Message);
    if (OnProfileFetched) OnProfileFetched(false, FNexusUserProfile());
}

bool FNexusNakamaProfile::EnsureClientAndSession(const FString& CallerName) const
{
    if (!Client)
    {
        NEXUS_ERROR("[NakamaProfile] %s: NakamaClient is null", *CallerName);
        return false;
    }
    if (!Session || Session->IsExpired())
    {
        NEXUS_ERROR("[NakamaProfile] %s: Session is null or expired — re-login required", *CallerName);
        return false;
    }
    return true;
}