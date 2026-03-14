#include "PulseChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PulseDistrict/Controller/PulsePlayerController.h"

void UPulseChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind the input box to commit function
    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.AddDynamic(this, &UPulseChatWidget::HandleTextCommitted);
    }

    // Bind to the Player Controller's incoming message delegate
    if (APulsePlayerController* PC = Cast<APulsePlayerController>(GetOwningPlayer()))
    {
        PC->OnChatMessageReceived.AddDynamic(this, &UPulseChatWidget::HandleChatMessageReceived);
    }
}

void UPulseChatWidget::NativeDestruct()
{
    if (APulsePlayerController* PC = Cast<APulsePlayerController>(GetOwningPlayer()))
    {
        PC->OnChatMessageReceived.RemoveDynamic(this, &UPulseChatWidget::HandleChatMessageReceived);
    }
    Super::NativeDestruct();
}

void UPulseChatWidget::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (APulsePlayerController* PC = Cast<APulsePlayerController>(GetOwningPlayer()))
        {
            PC->SendChatMessage(Text.ToString());
            
            // Give input focus completely back to the game
            PC->SetInputMode(FInputModeGameOnly());
            PC->SetShowMouseCursor(false);
        }

        if (ChatInputBox)
        {
            ChatInputBox->SetText(FText::GetEmpty());
        }
    }
}

void UPulseChatWidget::HandleChatMessageReceived(const FString& SenderName, const FString& Message)
{
    if (!ChatHistoryScrollBox) return;

    // Dynamically create a Text Block for the new message
    UTextBlock* NewMessage = NewObject<UTextBlock>(this);
    FString FormattedMessage = FString::Printf(TEXT("[%s]: %s"), *SenderName, *Message);
    NewMessage->SetText(FText::FromString(FormattedMessage));
    
    // Ensure long messages wrap correctly
    NewMessage->SetAutoWrapText(true);

    ChatHistoryScrollBox->AddChild(NewMessage);
    
    // Automatically scroll to the bottom so the newest message is visible
    ChatHistoryScrollBox->ScrollToEnd();
}