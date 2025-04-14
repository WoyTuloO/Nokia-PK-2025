#include "ViewingMessageState.hpp"
#include "ViewingMessageListState.hpp"
#include "NotConnectedState.hpp"
#include <stdexcept>


namespace ue{

ViewingMessageState::ViewingMessageState(Context &context, std::size_t messIdx) :
    BaseState(context, "ViewingMessageState"), viewingMessageIdx(messIdx)
{
    const auto &messages = context.smsStorage.getAllMessages();

    if(viewingMessageIdx >= messages.size()){
        context.setState<ViewingMessageListState>();
        return;
    }

    const auto& message = messages[viewingMessageIdx];

    if(!message.isRead)
        context.smsStorage.markAsRead(viewingMessageIdx);

    context.user.showMessageView(message);
}

void ViewingMessageState::handleUiBack(){
    context.setState<ViewingMessageListState>();
}

void ViewingMessageState::handleDisconnected(){
    context.setState<NotConnectedState>();
}

void ViewingMessageState::handleMessageReceive(common::PhoneNumber sender, std::string text){
    std::size_t messIdx = context.smsStorage.addMessage(sender, text);
    context.user.showNewMessage();
}




}

