#include "ViewingMessageListState.hpp"
#include "ViewingMessageState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"


namespace ue{

ViewingMessageListState::ViewingMessageListState(Context& context) : BaseState(context, "ViewingMessageListState")
{
    showList();
}

void ViewingMessageListState::showList(){
    currentMessages = context.smsStorage.getAllMessages();

    if(currentMessages.empty()){
        context.user.showAlert("SMS Inbox", "Empty");
        context.setState<ConnectedState>();
    }else{
        context.user.showMessageList(currentMessages);
    }
}

void ViewingMessageListState::handleUiAction(std::optional<std::size_t> selectedIdx){
    if(!selectedIdx.has_value()){
        return;
    }

    auto idx = selectedIdx.value();

    if(idx < currentMessages.size()){
        context.setState<ViewingMessageState>(idx);
    }else{
        logger.logError("Selected index is out of bounds");
    }

}

void ViewingMessageListState::handleUiBack(){
    context.setState<ConnectedState>();
}

void ViewingMessageListState:: handleMessageReceive(common::PhoneNumber sender, std::string text){
    auto idx = context.smsStorage.addMessage(sender, text);
    logger.logDebug("Message stored at idx: ", idx);
    context.user.showNewMessage();
    showList();

}

void ViewingMessageListState:: handleDisconnected(){
    context.setState<NotConnectedState>();
}


}