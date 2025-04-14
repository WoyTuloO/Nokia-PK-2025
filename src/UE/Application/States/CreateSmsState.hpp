#pragma once

#include "BaseState.hpp"
#include <optional>

namespace ue
{

    class CreateSmsState : public BaseState
    {
    public:
        CreateSmsState(Context &context);

        void handleUserAction(std::optional<std::size_t> selectedIndex) override;
        void handleUserBack() override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber sender, std::string text) override;
    };


}
