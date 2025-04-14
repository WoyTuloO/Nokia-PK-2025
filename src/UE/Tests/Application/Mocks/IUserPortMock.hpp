#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"

#include <string>
#include <vector>
#include "Application/SmsStorage.hpp"

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

    MOCK_METHOD(void, showMessageList, (const std::vector<SmsMessage>& message), (override));
    MOCK_METHOD(void, showMessageView, (const SmsMessage& message), (override));
    MOCK_METHOD(void, showAlert, (const std::string& title, const std::string& text), (override));


};

}
