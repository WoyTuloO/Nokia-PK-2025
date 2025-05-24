#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "Utils/todo.h"
#include <format>

namespace ue
{

TalkingState::TalkingState(Context& context, common::PhoneNumber to) : BaseState(context, "TalkingState"), to(to)
{
    using namespace std::literals::chrono_literals;

    logger.logInfo("Having call with: ", to);
    context.user.showCallTalkInterface();
    this->context.timer.startTimer(2min);
}

TalkingState::~TalkingState()
{
    /*
     * Theoretically, there should be destructor defined at TimerPort
     * that stops its timer, such that derived classes don't have to care about
     * resetting it. BUT, because I don't know whether there is
     * some miscellaneous interaction that needs to pass timer between
     * states it will be done this way. 
     */
    this->context.timer.stopTimer();
}

void TalkingState::handleUiAction([[maybe_unused]] std::optional<std::size_t> selectedIndex)
{
    std::string const& user_mesg = this->context.user.getCallText();
    logger.logInfo("User sent message: ", user_mesg);

    this->resetTimer();

    if (!user_mesg.empty())
    {
        this->context.user.appendCallText(std::format("[ME]: {}", user_mesg));
        this->context.bts.sendCallTalk(this->to, user_mesg);
        this->context.user.clearOutgoingCallText();
        this->logger.logDebug("Message: \"", user_mesg, "\" succesfully sent to recipient at ", this->to.value);
    }
    else
    {
        this->logger.logInfo("Empty message: sending aborted");
    }
}

void TalkingState::handleCallTalk(common::PhoneNumber to, const std::string& text)
{
    this->resetTimer();

    this->context.user.appendCallText(std::format("[{}]: {}", this->to.value, text));
    this->logger.logDebug("Message: \"", text, "\" succesfully received from caller at ", this->to.value);
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
    
    this->context.timer.stopTimer();
    context.bts.sendCallDropped(to);
    context.user.showEndedCall(to, "Call ended");
    context.setState<ConnectedState>();
}

void TalkingState::resetTimer(ue::ITimerPort::Duration inactivity_duration)
{
    logger.logDebug(std::format("Activity detected, resetting timer to {}", 
                                std::chrono::duration_cast<std::chrono::seconds>(inactivity_duration)));

    this->context.timer.stopTimer();
    this->context.timer.startTimer(inactivity_duration);
}

}