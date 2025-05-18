#include "TalkingState.hpp"
#include "ConnectedState.hpp"

namespace ue {

TalkingState::TalkingState(Context& context, common::PhoneNumber to)
    : BaseState(context, "TalkingState"), to(to) {
    logger.logInfo("Having call with: ", to);
    context.user.showCallInProgress(to);
}

void TalkingState::handleUiBack() {
    endCall();
}

void TalkingState::handleCallRequest(common::PhoneNumber from) {
    logger.logInfo("Drop new call, while talking: ", from);
    context.bts.sendCallDropped(from);
}

void TalkingState::endCall() {
    logger.logInfo("Ending call with: ", to);
    context.bts.sendCallDropped(to);
    context.user.showEndedCall(to, "Call ended");
    context.setState<ConnectedState>();
}

}