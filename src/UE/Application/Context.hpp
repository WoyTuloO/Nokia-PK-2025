#pragma once

#include "IEventsHandler.hpp"
#include "Logger/ILogger.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Ports/IUserPort.hpp"
#include "SmsStorage.hpp"
#include <memory>

namespace ue
{

struct Context
{
    common::ILogger& logger;
    IBtsPort& bts;
    IUserPort& user;
    ITimerPort& timer;
    std::unique_ptr<IEventsHandler> state{};

    SmsStorage smsStorage;
    common::PhoneNumber myPhoneNumber;

    template <typename State, typename... Arg>
    void setState(Arg&&... arg)
    {
        state = std::make_unique<State>(*this, std::forward<Arg>(arg)...);
    }
};

}
