#pragma once

#include "IEventsHandler.hpp"
#include "Logger/ILogger.hpp"
#include <memory>
#include "SmsStorage.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

    struct Context
    {
        common::ILogger& logger;
        IBtsPort& bts;
        IUserPort& user;
        ITimerPort& timer;
        SmsStorage smsStorage;
        std::unique_ptr<IEventsHandler> state{};

        template <typename State, typename ...Arg>
        void setState(Arg&& ...arg)
        {
            state = std::make_unique<State>(*this, std::forward<Arg>(arg)...);
        }
    };


}
