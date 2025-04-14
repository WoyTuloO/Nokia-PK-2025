#pragma once

#include <vector>
#include <string>
#include "SmsStorage.hpp"


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

    virtual void showMessageList(const std::vector<SmsMessage>& messages) = 0;
    virtual void showMessageView(const SmsMessage& message) = 0;
    virtual void showAlert(const std::string& title, const std::string& text) = 0;
};

}
