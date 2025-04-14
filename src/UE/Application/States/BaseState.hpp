#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"

namespace ue
{

class BaseState : public IEventsHandler
{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;

    void handleDisconnected() override;

    void handleMessageReceive(common::PhoneNumber sender, std::string text) override;
    void handleUiAction(std::optional<std::size_t> selectedIdx) override;
    void handleUiBack() override;


protected:
    Context& context;
    common::PrefixedLogger logger;
};

}
