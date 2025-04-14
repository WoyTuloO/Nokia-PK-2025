#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"

#include "IEventsHandler.hpp"
#include <vector>
#include "SmsStorage.hpp"

namespace ue
{

class UserPort : public IUserPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IEventsHandler& handler);
    void stop();

    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;

    void showNewMessage() override;

    void showMessageList(const std::vector<SmsMessage>& messages) override;
    void showMessageView(const SmsMessage& message) override;
    void showAlert(const std::string& title, const std::string& text) override;

private:
    void acceptCallback();
    void rejectCallback();

    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IEventsHandler* handler = nullptr;
    view_details::GuiViewMode currentViewMode = view_details::VM_DEFAULT;
};

}
