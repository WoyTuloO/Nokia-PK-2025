#pragma once

#include "SmsStorage.hpp"
#include <string>
#include <vector>

namespace ue
{

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showNewMessage() = 0;

    virtual void showListMessage(const std::vector<SmsMessage>& messages) = 0;
    virtual void showMessageView(const SmsMessage& message) = 0;
    virtual void showNotify(const std::string& name, const std::string& message) = 0;
    virtual void showMessageComp() = 0;
    virtual void showCallComp() = 0;

    // notify incoming call from the specified caller number
    virtual void showIncomingCall(const common::PhoneNumber& caller) = 0;
    // display the ongoing call status with the specified other party (callee or caller)
    virtual void showCallInProgress(const common::PhoneNumber& otherPhoneNumber) = 0;
    // display calltalk gui interface
    virtual void showCallTalkInterface() = 0;
    // notify the user that the call has ended and why
    virtual void showEndedCall(const common::PhoneNumber& otherPhoneNumber, const std::string& reason) = 0;
    virtual void showCallFailed(const common::PhoneNumber& otherPhoneNumber, const std::string& errorMessage) = 0;
    virtual void showAlertPeerUnknownRecipient(const common::PhoneNumber& otherPhoneNumber) = 0;

    [[deprecated("There no longer exists Call submenu")]] virtual void showCallMenu() = 0;

    [[nodiscard]] virtual common::PhoneNumber getMessageRecipient() const = 0;
    [[nodiscard]] virtual std::string getMessageText() const = 0;
    [[nodiscard]] virtual common::PhoneNumber getCallRecipient() const = 0;

    [[nodiscard]] virtual std::string getCallText() const = 0;
    virtual void clearIncomingCallText() = 0;
    virtual void clearOutgoingCallText() = 0;
    virtual void appendCallText(std::string const& message) = 0;
};

}
