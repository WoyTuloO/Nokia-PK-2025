#pragma once

#include "BaseState.hpp"
#include <optional>

namespace ue
{

    class ViewSingleSmsState : public BaseState
    {
    public:
        ViewSingleSmsState(Context &context, std::size_t smsIndex);

        void handleUserBack() override;
        void handleUserAction(std::optional<std::size_t> selectedIndex) override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber sender, std::string text) override;

    private:
        std::size_t smsIndex;
    };

}
