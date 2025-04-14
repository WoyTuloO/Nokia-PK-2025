#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ViewingMessageListState.hpp"
#include "ViewingMessageState.hpp"

#include <vector>

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
}

void ConnectedState::handleDisconnected(){
    context.setState<NotConnectedState>();
}

void ConnectedState::handleMessageReceive(common::PhoneNumber sender, std::string text){
    logger.logInfo("SMS received from: ", sender, ", content : ", text);

    std::size_t smsIndex = context.smsStorage.addMessage(sender, text);
    logger.logDebug("SMS stored at index: ", smsIndex);

    context.user.showNewMessage();
}

void ConnectedState:: handleUiAction(std::optional<std::size_t> selectedIdx){
    if(!selectedIdx.has_value())
        return;

    auto idx = selectedIdx.value();

    if(idx == 1){
        context.setState<ViewingMessageListState>();
    }else{
        logger.logInfo("Unknown index selected: ", idx);
    }



}

void ConnectedState:: handleUiBack(){
        logger.logInfo("Back action in main menu ignored");

}

}
