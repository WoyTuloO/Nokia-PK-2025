#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);
    void handleDisconnected() override;

    void handleSmsReceive(common::PhoneNumber sender, std::string text) override;
};

}
