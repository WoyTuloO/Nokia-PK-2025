#pragma once

#include "BaseState.hpp"
#include <vector>

namespace ue
{

    class ViewListSmsState : public BaseState
    {
    public:
        ViewListSmsState(Context& context);

        void handleUserAction(std::optional<std::size_t> selectedIndex) override;
        void handleUserBack() override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber sender, std::string text) override;

    private:
        std::vector<SmsMessage> currentSmsList;
    };


}
