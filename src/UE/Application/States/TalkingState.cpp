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
    this->context.user.clearIncomingCallText();
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

void TalkingState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    if (!selectedIndex.has_value())
    {
        std::string const& user_mesg = this->context.user.getCallText();
        logger.logInfo("User sent message: ", user_mesg);

        this->resetTimer();

        if (!user_mesg.empty())
        {
            this->context.user.appendCallText(std::format("[ME]: {:s}", user_mesg));
            this->context.bts.sendCallTalk(this->to, user_mesg);
            this->context.user.clearOutgoingCallText();
            this->logger.logDebug(std::format("Message: \"{:s}\" succesfully sent to recipient at {:0>3d}", user_mesg, this->to.value));
        }
        else
        {
            this->logger.logInfo("Empty message: sending aborted");
        }
    }
    else if (selectedIndex.value() == 1LU)
    {
        this->logger.logInfo("User accepted BTS sending UnknownRecipient");
        this->context.timer.stopTimer();
        this->context.setState<ConnectedState>();
    }
}

void TalkingState::handleCallTalk(common::PhoneNumber to, const std::string& text)
{
    this->resetTimer();

    this->context.user.appendCallText(std::format("[{:0>3d}]: {:s}", this->to.value, text));
    this->logger.logDebug(std::format("Message: \"{:s}\" succesfully received from caller at {:0>3d}", text, this->to.value));
}

void TalkingState::handleUiBack()
{
    endCall();
}

void TalkingState::handleCallRequest(common::PhoneNumber from)
{
    logger.logDebug("4.2.9.4 UE receives Call Request, while having Call (Talking)");
    logger.logInfo("Drop new call, while talking: ", from);

    if (from == this->to)
    {
        context.bts.sendCallDropped(from);
    }
}

void TalkingState::handleCallDropped(common::PhoneNumber from)
{
    logger.logInfo("Peer explicitly disconnected");
    this->context.timer.stopTimer();
    context.setState<ConnectedState>();
}

void TalkingState::handleUnknownRecipient(common::PhoneNumber from)
{
    using namespace std::chrono_literals;

    logger.logInfo("Peer not connected to BTS");
    this->context.timer.stopTimer();
    this->context.timer.startTimer(5s);
    this->context.user.showAlertPeerUnknownRecipient(from);
}

void TalkingState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    this->logger.logDebug("4.2.9.3 UE receives SMS, while having Call (Talking)");

    logger.logInfo("TalkingState: incoming SMS from ", from);
    context.smsStorage.addMessage(from, text);
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