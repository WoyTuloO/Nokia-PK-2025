#pragma once

#include "BaseState.hpp"
#include "IUeGui.hpp"
#include "SmsStorage.hpp"
#include <optional>
#include <vector>

namespace ue
{

class ViewListSmsState : public BaseState
{
public:
    explicit ViewListSmsState(Context& context);

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleDisconnected() override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;
    void handleCallRequest(common::PhoneNumber from) override;

private:
    void refreshList();
    void logSmsListDebug();
    bool isRead(const SmsMessage& msg) const;

    std::vector<SmsMessage> messages_;
};

}
