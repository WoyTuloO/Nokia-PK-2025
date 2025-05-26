#pragma once

#include "Messages/PhoneNumber.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Ports/IUserPort.hpp"
#include <optional>

namespace ue
{
namespace view_details
{

enum class GuiViewMode : std::uint8_t
{
    Default,         // 0
    Menu,            // 1
    Message_list,    // 2
    Message_view,    // 3 [deprecated]
    Message_compose, // 4
    Message_menu,    // 5
    Call_menu,       // 6 [deprecated]
    Call_compose,    // 7
    Call_outgoing,   // 8
    Call_incoming,   // 9
    Call_talk,       // 10
    Call_alert_UR    // 11
};

}

class IEventsHandler : public IBtsEventsHandler, public ITimerEventsHandler, public IUserEventsHandler
{
public:
    virtual ~IEventsHandler() = default;

    virtual void handleUiAction(std::optional<std::size_t> selectedIndex) = 0;
    virtual void handleUiBack() = 0;
    virtual void handleMessageComposeResult(common::PhoneNumber reciver, const std::string& text) = 0;
};

}