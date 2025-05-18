#pragma once
#include "BaseState.hpp"

namespace ue {

class TalkingState : public BaseState {
public:
    TalkingState(Context& context, common::PhoneNumber to);
    void handleUiBack() override;
    void handleCallRequest(common::PhoneNumber from) override;

private:
    common::PhoneNumber to;
    void endCall();
};

}