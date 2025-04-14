#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "SmsStorage.hpp"
#include <optional>

namespace ue
{

using common::PhoneNumber;
using common::ILogger;

class Application : public IEventsHandler
{
public:
    Application(PhoneNumber phoneNumber,
                ILogger& iLogger,
                IBtsPort& bts,
                IUserPort& user,
                ITimerPort& timer);
    ~Application();

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;

    void handleDisconnected() override;

    void handleMessageReceive( common::PhoneNumber sender, std::string text) override;

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

  void handleMessageSentResult(common::PhoneNumber to, bool success) override;
  void handleMessageComposeResult(common::PhoneNumber receiver, const std::string& text) override;


private:
    Context context;
    common::PrefixedLogger logger;

};

}
