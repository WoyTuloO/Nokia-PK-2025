#include "ViewListSmsState.hpp"
#include "ViewSingleSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    ViewListSmsState::ViewListSmsState(Context& context)
        : BaseState(context, "ViewListSmsState")
    {
        logger.logInfo("Entering SMS List view.");
        currentSmsList = context.smsStorage.getAllMessages();
        if (currentSmsList.empty())
        {
            logger.logInfo("SMS list is empty.");
            context.user.showAlert("Inbox", "No messages available.");
            context.setState<ConnectedState>();
        }
        else
        {
            context.user.showSmsList(currentSmsList);
        }
    }

    void ViewListSmsState::handleUserAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value())
        {
            logger.logInfo("No SMS selected from list.");
            return;
        }
        std::size_t index = selectedIndex.value();
        if (index < currentSmsList.size())
        {
            logger.logInfo("SMS selected at index: ", index);
            context.setState<ViewSingleSmsState>(index);
        }
        else
        {
            logger.logError("Invalid selection index: ", index);
        }
    }

    void ViewListSmsState::handleUserBack()
    {
        logger.logInfo("Returning to main menu from SMS list view.");
        context.setState<ConnectedState>();
    }

    void ViewListSmsState::handleSmsReceive(common::PhoneNumber sender, std::string text)
    {
        logger.logInfo("SMS received (from: ", sender, ") while viewing list.");
        context.smsStorage.addMessage(sender, text);
        context.user.showNewMessage();
        currentSmsList = context.smsStorage.getAllMessages();
        context.user.showSmsList(currentSmsList);
    }

    void ViewListSmsState::handleDisconnected()
    {
        logger.logInfo("Disconnected while viewing SMS list.");
        context.setState<NotConnectedState>();
    }

}
