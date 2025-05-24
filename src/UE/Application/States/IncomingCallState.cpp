#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

IncomingCallState::IncomingCallState(Context& context, common::PhoneNumber caller)
    : BaseState(context, "IncomingCallState"), callerNumber(caller)
{
    using namespace std::literals::chrono_literals;

    logger.logInfo("Waiting for accept call from: ", callerNumber);
    context.user.showIncomingCall(callerNumber);
    context.timer.startTimer(30s);
}

void IncomingCallState::handleUiAction([[maybe_unused]] std::optional<std::size_t> selectedIndex)
{
    logger.logInfo("User tapped: Accept Call");
    acceptCall();
}

void IncomingCallState::handleUiBack()
{
    rejectCall();
}

void IncomingCallState::handleTimeout()
{
    logger.logInfo("Call timed out");
    context.bts.sendCallDropped(callerNumber);
    context.setState<ConnectedState>();
}

void IncomingCallState::handleCallDropped(common::PhoneNumber from)
{
    logger.logInfo("Call dropped by remote peer");
    context.timer.stopTimer();
    context.setState<ConnectedState>();
}

void IncomingCallState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    this->logger.logDebug("4.2.6.3 UE receives SMS, while receiving CallRequest");

    logger.logInfo("IncomingCallState: incoming SMS from ", from);
    context.smsStorage.addMessage(from, text);
}

void IncomingCallState::handleDisconnected()
{
    this->logger.logDebug("4.2.6.2 UE connection to BTS was dropped while receiving CallRequest");

    this->logger.logError("Unexpectedly disconnected from BTS");
    this->context.timer.stopTimer();
    this->context.setState<NotConnectedState>();
}


void IncomingCallState::acceptCall()
{
    logger.logInfo("Call accepted");
    context.bts.sendCallAccepted(callerNumber);
    context.timer.stopTimer();
    context.setState<TalkingState>(callerNumber);
}

void IncomingCallState::rejectCall()
{
    logger.logInfo("Call rejected");
    context.bts.sendCallDropped(callerNumber);
    context.timer.stopTimer();
    context.setState<ConnectedState>();
}
}