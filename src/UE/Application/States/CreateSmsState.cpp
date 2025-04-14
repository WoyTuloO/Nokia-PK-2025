#include "CreateSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    CreateSmsState::CreateSmsState(Context &context)
        : BaseState(context, "CreateSmsState")
    {
        logger.logInfo("Entering SMS Composition state.");
        context.user.showSmsCompose();
    }

    void CreateSmsState::handleUserAction(std::optional<std::size_t> /*selectedIndex*/)
    {
        logger.logInfo("Sending SMS initiated.");
        auto recipient = context.user.getSmsRecipient();
        auto text = context.user.getSmsText();
        if (!recipient.isValid() || text.empty())
        {
            logger.logInfo("Cannot send SMS: Invalid recipient or empty text.");
            context.user.showAlert("Error", "Invalid recipient or empty text.");
            return;
        }
        context.smsStorage.addMessage(recipient, text);
        context.bts.sendSms(recipient, text);
        logger.logInfo("SMS sent to: ", recipient);
        context.setState<ConnectedState>();
    }

    void CreateSmsState::handleUserBack()
    {
        logger.logInfo("SMS composition canceled by user.");
        context.setState<ConnectedState>();
    }

    void CreateSmsState::handleDisconnected()
    {
        logger.logInfo("Disconnected while composing SMS.");
        context.user.showAlert("Disconnected", "Connection lost during SMS composition.");
        context.setState<NotConnectedState>();
    }

    void CreateSmsState::handleSmsReceive(common::PhoneNumber sender, std::string text)
    {
        logger.logInfo("SMS received from ", sender, " during composition.");
        context.smsStorage.addMessage(sender, text);
        context.user.showNewMessage();
    }


}
