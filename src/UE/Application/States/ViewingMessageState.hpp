#pragma once

#include "BaseState.hpp"


namespace ue{

class ViewingMessageState: public BaseState{
    public:
        ViewingMessageState(Context& context, std::size_t messIdx);
        void handleUiBack() override;
        void handleDisconnected() override;

        void handleMessageReceive(common:: PhoneNumber sender, std::string text) override;

    private:
        std::size_t viewingMessageIdx;
};


}