#pragma once

#include "Ports/IUserPort.hpp"
#include <gmock/gmock.h>

namespace ue
{

class IUserEventsHandlerMock : public IUserEventsHandler
{
public:
    IUserEventsHandlerMock();
    ~IUserEventsHandlerMock() override;
};

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showNewMessage, (), (override));

    MOCK_METHOD(void, showListMessage, (const std::vector<SmsMessage>& messages), (override));
    MOCK_METHOD(void, showMessageView, (const SmsMessage& message), (override));
    MOCK_METHOD(void, showNotify, (const std::string& name, const std::string& message), (final));
    MOCK_METHOD(void, showMessageComp, (), (final));
    MOCK_METHOD(void, showCallComp, (), (final));

    MOCK_METHOD(void, showIncomingCall, (const common::PhoneNumber& caller), (override));
    MOCK_METHOD(void, showCallInProgress, (const common::PhoneNumber& otherPhoneNumber), (override));
    MOCK_METHOD(void, showCallTalkInterface, (), (override));
    MOCK_METHOD(void,
                showEndedCall,
                (const common::PhoneNumber& otherPhoneNumber, const std::string& reason),
                (override));
    MOCK_METHOD(void,
                showCallFailed,
                (const common::PhoneNumber& otherPhoneNumber, const std::string& errorMessage),
                (override));

    MOCK_METHOD(void, showCallMenu, (), (override));
    MOCK_METHOD(void, showAlertPeerUnknownRecipient, (const common::PhoneNumber& otherPhoneNumber), (override));

    MOCK_METHOD(common::PhoneNumber, getMessageRecipient, (), (const, final));
    MOCK_METHOD(std::string, getMessageText, (), (const, final));
    MOCK_METHOD(common::PhoneNumber, getCallRecipient, (), (const, override));
    MOCK_METHOD(std::string, getCallText, (), (const, override));
    MOCK_METHOD(void, clearIncomingCallText, (), (override));
    MOCK_METHOD(void, clearOutgoingCallText, (), (override));
    MOCK_METHOD(void, appendCallText, (std::string const& message), (override));
};

}
