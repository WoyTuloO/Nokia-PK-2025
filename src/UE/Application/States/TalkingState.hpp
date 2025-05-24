#pragma once
#include "BaseState.hpp"

namespace ue {

class TalkingState : public BaseState {
public:
    TalkingState(Context& context, common::PhoneNumber to);
    ~TalkingState() override;
    /* Reenact rule of three
     * I don't care that they don't keep inactivity time and instead
     * reset timer to two minutes
     */
    TalkingState(TalkingState const& o) = default;
    TalkingState& operator=(TalkingState const& o) = default;

    void handleUiBack() override;
    void handleTimeout() override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleUnknownRecipient(common::PhoneNumber from) override;
    void handleCallTalk(common::PhoneNumber to, const std::string& text) override;
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;

private:
    common::PhoneNumber to;
    void endCall();

    void resetTimer(ue::ITimerPort::Duration inactivity_duration = std::chrono::minutes(2));
};

}