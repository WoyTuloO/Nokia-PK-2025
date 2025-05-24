#include "CallState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"
#include "IncomingCallState.hpp"
#include "NotConnectedState.hpp"
#include "Utils/todo.h"

#include <format>

namespace ue
{
DiallingState::DiallingState(Context& context) : BaseState(context, "DiallingState")
{
    this->logger.logInfo("Entered DiallingState");
    this->context.user.showCallComp();
};

void DiallingState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    logger.logInfo("DiallingState: User tapped Call");

    auto number_to_call = this->context.user.getCallRecipient();

    if (!this->validateCallNumber())
    {
        logger.logInfo(std::format("DiallingState: invalid recipient ({:0>3d})", number_to_call.value));
        context.user.showNotify("Error", "Invalid recipient");
        return;
    }

    logger.logInfo(std::format("DiallingState: trying to call ({:0>3d})", number_to_call.value));
    this->context.setState<OutgoingDiallingState>(number_to_call);
}

void DiallingState::handleUiBack()
{
    logger.logInfo("Exiting DiallingState");
    this->context.setState<ConnectedState>();
}

void DiallingState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    this->logger.logDebug("4.2.7.3 UE receives SMS, while sending Call Request");

    logger.logInfo("DiallingState: incoming SMS from ", from);
    context.smsStorage.addMessage(from, text);
}

void DiallingState::handleCallRequest(common::PhoneNumber from)
{
    this->logger.logDebug("4.2.7.4 UE receives Call Request, while sending Call Request");
    this->logger.logInfo(std::format("Call request from number: {:0>3}, dropping call composing", from.value));

    this->context.setState<IncomingCallState>(from);
}

void DiallingState::handleDisconnected()
{
    this->logger.logDebug("4.2.7.2 UE connection to BTS was dropped while sending Call Request");

    this->logger.logError("Unexpectedly disconnected from BTS");
    this->context.timer.stopTimer();
    this->context.setState<NotConnectedState>();
}

constexpr bool DiallingState::validateCallNumber() const noexcept
{
    return this->context.user.getCallRecipient().isValid();
}


}

namespace ue
{
OutgoingDiallingState::OutgoingDiallingState(Context& context, common::PhoneNumber to) : BaseState(context, "DiallingState"), number_to_call{to}
{
    using namespace std::literals::chrono_literals;

    this->logger.logInfo("Entered OutgoingDiallingState");
    this->context.bts.sendCallRequest(number_to_call);
    this->context.timer.startTimer(60s);
    this->context.user.showCallInProgress(this->number_to_call);
}

OutgoingDiallingState::~OutgoingDiallingState()
{
    this->context.timer.stopTimer();
}

void OutgoingDiallingState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    if (!selectedIndex.has_value())
    {
        this->logger.logDebug("Accepting while calling does nothing - ignoring...");
    }
    else if (selectedIndex.value() == 1LU)
    {
        this->logger.logInfo("User accepted BTS sending UnknownRecipient");
        this->context.timer.stopTimer();
        this->context.setState<ConnectedState>();
    }
}

void OutgoingDiallingState::handleUiBack()
{
    this->context.logger.logInfo("Dialling cancelled by the user");
    this->context.timer.stopTimer();
    this->context.bts.sendCallDropped(this->number_to_call);
    this->context.setState<ConnectedState>();
}

void OutgoingDiallingState::handleTimeout()
{
    this->context.bts.sendCallDropped(this->number_to_call);
    this->context.logger.logInfo("Dialling timed out");
    this->context.setState<ConnectedState>();
}

void OutgoingDiallingState::handleCallAccepted(common::PhoneNumber from)
{
    logger.logInfo("Successfully connected");
    this->context.timer.stopTimer();
    this->context.setState<TalkingState>(from);
}

void OutgoingDiallingState::handleCallDropped(common::PhoneNumber from)
{
    logger.logInfo("Callee dropped the call");
    this->context.timer.stopTimer();
    this->context.setState<ConnectedState>();
}

void OutgoingDiallingState::handleUnknownRecipient(common::PhoneNumber from)
{
    using namespace std::chrono_literals;

    logger.logInfo("Recipient not found");
    this->context.timer.stopTimer();
    this->context.timer.startTimer(5s);
    this->context.user.showAlertPeerUnknownRecipient(from);
}

void OutgoingDiallingState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    this->logger.logDebug("4.2.7.3 UE receives SMS, while sending Call Request");

    logger.logInfo("OutgoingDiallingState: incoming SMS from ", from);
    context.smsStorage.addMessage(from, text);
}

void OutgoingDiallingState::handleCallRequest(common::PhoneNumber from)
{
    this->logger.logDebug("4.2.7.4 UE receives Call Request, while sending Call Request");
    this->logger.logInfo(std::format("Call request from number: {:0>3}, dropping current call with {:0>3}", from.value, this->number_to_call.value));

    this->context.timer.stopTimer();
    this->context.bts.sendCallDropped(this->number_to_call);
    this->context.setState<IncomingCallState>(from);
}

void OutgoingDiallingState::handleDisconnected()
{
    this->logger.logDebug("4.2.7.2 UE connection to BTS was dropped while sending Call Request");

    this->logger.logError("Unexpectedly disconnected from BTS");
    this->context.timer.stopTimer();
    this->context.setState<NotConnectedState>();
}

}