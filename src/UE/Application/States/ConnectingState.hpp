#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectingState : public BaseState
{
public:
    explicit ConnectingState(Context& context);

    void handleAttachAccept() override;
    void handleAttachReject() override;

    void handleTimeout() override;
    void handleDisconnected() override;

private:
    common::BtsId btsId;
};

}
