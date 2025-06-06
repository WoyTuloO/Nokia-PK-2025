#pragma once
#include "BaseState.hpp"

namespace ue
{

class IncomingCallState : public BaseState
{
public:
    IncomingCallState(Context& context, common::PhoneNumber caller);
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleTimeout() override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleDisconnected() override;

private:
    common::PhoneNumber callerNumber;
    void acceptCall();
    void rejectCall();
};

}