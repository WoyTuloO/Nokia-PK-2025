#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "Utils/todo.h"

namespace ue
{

TalkingState::TalkingState(Context& context, common::PhoneNumber to) : BaseState(context, "TalkingState"), to(to)
{
    using namespace std::literals::chrono_literals;

    logger.logInfo("Having call with: ", to);
    context.user.showCallTalkInterface();
    this->context.timer.startTimer(2min);
}

void TalkingState::handleUiBack()
{
    endCall();
}

void TalkingState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Drop new call, while talking: ", from);
    context.bts.sendCallDropped(from);
}

void TalkingState::handleCallDropped(common::PhoneNumber from)
{
    logger.logInfo("Peer explicitly disconnected");
    this->context.timer.stopTimer();
    context.setState<ConnectedState>();
}

void TalkingState::handleUnknownRecipient(common::PhoneNumber from)
{
    logger.logInfo("Peer not connected to BTS");
    this->context.timer.stopTimer();
    TODO(showPartnerNotAvailable);
    context.setState<ConnectedState>();
}

void TalkingState::handleTimeout()
{
    logger.logInfo("Connection timed out from inactivity");
    this->context.bts.sendCallDropped(this->to);
    context.setState<ConnectedState>();
}

void TalkingState::endCall()
{
    logger.logInfo("Ending call with: ", to);
    context.bts.sendCallDropped(to);
    context.user.showEndedCall(to, "Call ended");
    context.setState<ConnectedState>();
}

}