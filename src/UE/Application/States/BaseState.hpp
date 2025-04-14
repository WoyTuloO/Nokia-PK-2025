#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"
#include <optional>

namespace ue
{

    class BaseState : public IEventsHandler
    {
    public:
        BaseState(Context& context, const std::string& name);
        virtual ~BaseState() override;

        virtual void handleTimeout() override;

        virtual void handleSib(common::BtsId btsId) override;
        virtual void handleAttachAccept() override;
        virtual void handleAttachReject() override;
        virtual void handleDisconnected() override;
        virtual void handleSmsReceive(common::PhoneNumber sender, std::string text) override;

        virtual void handleUserAction(std::optional<std::size_t> selectedIndex) override
        {
            logger.logError("Unexpected UI action received.");
        }
        virtual void handleUserBack() override
        {
            logger.logError("Unexpected UI back action received.");
        }

    protected:
        Context& context;
        common::PrefixedLogger logger;
    };


}
