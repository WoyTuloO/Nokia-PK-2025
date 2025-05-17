#include "CallState.hpp"
#include "ConnectedState.hpp"
#include "Utils/todo.h"

namespace ue
{
DiallingState::DiallingState(Context& context) : BaseState(context, "DiallingState")
{
    this->logger.logInfo("Entered DiallingState");
    this->context.user.showCallComp();
};

void DiallingState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    using namespace std::literals::chrono_literals;

    logger.logInfo("DiallingState: User tapped Call");

    this->number_to_call = this->context.user.getCallRecipient();

    if (!this->validateCallNumber())
    {
        logger.logInfo("DiallingState: invalid recipient (", this->number_to_call, ")");
        context.user.showNotify("Error", "Invalid recipient");
        return;
    }

    logger.logInfo("DiallingState: trying to call (", this->number_to_call, ")");
    
    this->context.bts.sendCallRequest(this->number_to_call);
    this->context.timer.startTimer(60s);

    TODO(Call accept)

    // logger.logInfo("Successfully connected");
    // this->context.timer.stopTimer();

    TODO(Set state to TalkingState)
    TODO(Consider timeout)
}

void DiallingState::handleUiBack()
{
    this->context.logger.logInfo("DiallingState: Dialling cancelled by the user");
    this->context.setState<ConnectedState>();
}

void DiallingState::handleTimeout()
{
    this->context.bts.sendCallDropped(this->number_to_call);
    this->context.logger.logInfo("DiallingState: Dialling timed out");
    TODO(show user alert that call timed out)
    this->context.setState<ConnectedState>();
}


constexpr bool DiallingState::validateCallNumber() const noexcept
{
    return this->context.user.getCallRecipient().isValid();
}
}