#include "BaseState.hpp"

namespace ue
{

BaseState::BaseState(Context &context, const std::string &name)
    : context(context),
      logger(context.logger, "[" + name + "]")
{
    logger.logDebug("entry");
}

BaseState::~BaseState()
{
    logger.logDebug("exit");
}

void BaseState::handleTimeout()
{
    logger.logError("Uexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId)
{
    logger.logError("Uexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept()
{
    logger.logError("Uexpected: handleAttachAccept");
}

void BaseState::handleAttachReject()
{
    logger.logError("Uexpected: handleAttachReject");
}
void BaseState::handleDisconnected(){
    logger.logError("Unexpected: handleDisconnected");
}

void BaseState::handleMessageReceive(common::PhoneNumber sender, std::string text){
    logger.logError("Unexpected: handleMessageReceive() from: ", sender, " content: ", text);
}

void BaseState::handleUiAction(std::optional<std::size_t> selectedIdx){
    logger.logError("Unexpected: handleUiAction(), index: ", (selectedIdx.has_value() ? std::to_string(selectedIdx.value()) : "none"));
}

void BaseState::handleUiBack(){
    logger.logError("Unexpected: handleUiBack()");
}

}
