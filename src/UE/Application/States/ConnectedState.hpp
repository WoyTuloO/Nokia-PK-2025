#pragma once

#include "BaseState.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);
    void handleDisconnected() override;

    void handleMessageReceive(common::PhoneNumber sender, std::string text) override;

    void handleUiAction(std::optional<std::size_t> selectedIdx) override;
    void handleUiBack() override;
};

}
