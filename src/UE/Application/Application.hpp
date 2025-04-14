#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "SmsStorage.hpp"

namespace ue
{

    using common::ILogger;
    using common::PhoneNumber;

    class Application : public IEventsHandler
    {
    public:
        Application(PhoneNumber phoneNumber,
                    ILogger &iLogger,
                    IBtsPort &bts,
                    IUserPort &user,
                    ITimerPort &timer);
        ~Application();

        void handleTimeout() override;

        void handleSib(common::BtsId btsId) override;
        void handleAttachAccept() override;
        void handleAttachReject() override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber sender, std::string text) override;
        void handleSmsSentResult(common::PhoneNumber to, bool success) override;
        void handleSmsComposeResult(common::PhoneNumber recipient, const std::string& text) override;

        void handleUserAction(std::optional<std::size_t> selectedIndex) override;
        void handleUserBack() override;

    private:
        Context context;
        common::PrefixedLogger logger;
    };


}
