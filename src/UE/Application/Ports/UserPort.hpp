#pragma once

#include "IEventsHandler.hpp"
#include "IUeGui.hpp"
#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "SmsStorage.hpp"
#include "UeGui/ICallMode.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include <optional>
#include <vector>

// Disturbing comment

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

    void showNewMessageReceived();

    void showListMessage(const std::vector<SmsMessage>& messages) override;
    void showMessageView(const SmsMessage& message) override;
    void showNotify(const std::string& name, const std::string& message) override;
    void showMessageComp() override;
    void showCallComp() override;

    void showIncomingCall(const common::PhoneNumber& caller) override;
    void showCallInProgress(const common::PhoneNumber& otherPhoneNumber) override;
    void showCallTalkInterface() override;
    void showEndedCall(const common::PhoneNumber& otherPhoneNumber, const std::string& reason) override;
    void showCallFailed(const common::PhoneNumber& otherPhoneNumber, const std::string& errorMessage) override;
    void showAlertPeerUnknownRecipient(const common::PhoneNumber& otherPhoneNumber) override;

    void showCallMenu() override;

    [[nodiscard]] common::PhoneNumber getMessageRecipient() const override;
    [[nodiscard]] std::string getMessageText() const override;
    [[nodiscard]] common::PhoneNumber getCallRecipient() const override;
    [[nodiscard]] std::string getCallText() const override;
    void clearIncomingCallText() override;
    void clearOutgoingCallText() override;
    void appendCallText(std::string const& message) override;

private:
    void acceptCallback();
    void rejectCallback();
    void messageCallback();

    common::PrefixedLogger logger;
    // IUeGui should probably be unique_ptr
    // or maybe constructor should take rvalues?
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IEventsHandler* handler = nullptr;
    view_details::GuiViewMode currentViewMode{ view_details::GuiViewMode::Default };
};

}
