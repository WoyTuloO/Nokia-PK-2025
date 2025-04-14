#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <optional>

namespace ue
{

    class IEventsHandler : public IBtsEventsHandler,
                           public ITimerEventsHandler,
                           public IUserEventsHandler
    {
    public:
        virtual ~IEventsHandler() = default;

        virtual void handleUserAction(std::optional<std::size_t> selectedIndex) = 0;
        virtual void handleUserBack() = 0;
    };


}
