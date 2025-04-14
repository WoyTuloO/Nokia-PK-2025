#pragma once

#include "BaseState.hpp"

namespace ue{

class ViewingMessageListState : public BaseState{
    public:
        ViewingMessageListState(Context& context);

        void handleUiAction(std::optional<std::size_t> selectedIdx) override;
        void handleUiBack() override;
        void handleDisconnected() override;

        void handleMessageReceive(common::PhoneNumber sender, std::string text) override;

    private:
        void showList();
        std::vector<SmsMessage> currentMessages;


};



}