#include "IncomingCallState.hpp"
#include "TalkingState.hpp"
#include "ConnectedState.hpp"

namespace ue {

IncomingCallState::IncomingCallState(Context& context, common::PhoneNumber caller)
    : BaseState(context, "IncomingCallState"), callerNumber(caller) {
    logger.logInfo("Waiting for accept call from: ", callerNumber);
    context.user.showIncomingCall(callerNumber);
    context.timer.startTimer(60s);
}

void IncomingCallState::handleUiAction(std::optional<std::size_t> selectedIndex) {
    if (!selectedIndex.has_value()) return;

    switch (selectedIndex.value()) {
        case 0:
            acceptCall();
            break;
        case 1:
            rejectCall();
            break;
        default:
            logger.logError("Unknown UI actoin in IncomingCallState");
            break;
    }
}

void IncomingCallState::handleUiBack() {
    rejectCall();
}

void IncomingCallState::handleTimeout() {
    logger.logInfo("Wainting for accept call timeout");
    rejectCall();
}

void IncomingCallState::acceptCall() {
    logger.logInfo("Call accepted");
    context.bts.sendCallAccepted(callerNumber);
    context.setState<TalkingState>(callerNumber);
}

void IncomingCallState::rejectCall() {
    logger.logInfo("Call rejected");
    context.bts.sendCallDropped(callerNumber);
    context.setState<ConnectedState>();
}