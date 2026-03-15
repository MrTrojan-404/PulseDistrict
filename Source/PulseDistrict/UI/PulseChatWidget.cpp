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
    APulsePlayerController* PC = Cast<APulsePlayerController>(GetOwningPlayer());
    if (!PC) return;

    // If the player presses Enter
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (!Text.IsEmpty())
        {
            PC->SendChatMessage(Text.ToString());
        }
        
        // Clear the box and close the chat
        if (ChatInputBox) ChatInputBox->SetText(FText::GetEmpty());
        PC->CloseChat();
    }
    // If the player presses Escape or clicks away from the box
    else if (CommitMethod == ETextCommit::OnCleared || CommitMethod == ETextCommit::OnUserMovedFocus)
    {
        if (ChatInputBox) ChatInputBox->SetText(FText::GetEmpty());
        PC->CloseChat();
    }
}

void UPulseChatWidget::HandleChatMessageReceived(const FString& SenderName, const FString& Message)
{
    if (!ChatHistoryScrollBox) return;

    UTextBlock* NewMessage = NewObject<UTextBlock>(this);
    FString FormattedMessage = FString::Printf(TEXT("[%s]: %s"), *SenderName, *Message);
    NewMessage->SetText(FText::FromString(FormattedMessage));
    NewMessage->SetAutoWrapText(true);
    
    // Apply the font you selected in the Widget Blueprint!
    if (ChatFont.HasValidFont())
    {
        NewMessage->SetFont(ChatFont);
    }

    ChatHistoryScrollBox->AddChild(NewMessage);
    ChatHistoryScrollBox->ScrollToEnd();
}

void UPulseChatWidget::FocusChatInput()
{
    if (ChatInputBox)
    {
        ChatInputBox->SetKeyboardFocus();
    }
}
