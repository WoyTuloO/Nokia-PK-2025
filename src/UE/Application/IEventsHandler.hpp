#pragma once

#include "Ports/ITimerPort.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include <optional>
#include "Messages/PhoneNumber.hpp"



namespace ue
{



class IEventsHandler : public IBtsEventsHandler,
                       public IUserEventsHandler,
                       public ITimerEventsHandler
{

    public:
        virtual ~IEventsHandler() = default;

        virtual void handleUiAction(std::optional<std::size_t> selectedIdx) = 0;
        virtual void handleUiBack() = 0;
        virtual void handleMessageReceive(common::PhoneNumber sender, std::string text) = 0;

        virtual void handleMessageSentResult(common::PhoneNumber to, bool success) = 0;
        virtual void handleMessageComposeResult(common::PhoneNumber recipient, const std::string& text) = 0;

};


    namespace view_details{
        using GuiViewMode = int;
        constexpr GuiViewMode VM_DEFAULT =0;
        constexpr GuiViewMode VM_MENU =1;
        constexpr GuiViewMode VM_MESSAGE_LIST =2;
        constexpr GuiViewMode VM_MESSAGE_VIEW=3;
        constexpr GuiViewMode VM_SMS_COMPOSE=4;
        constexpr GuiViewMode VM_SMS_MENU=5;

    }



}
