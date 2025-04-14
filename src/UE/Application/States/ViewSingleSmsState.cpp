#include "ViewSingleSmsState.hpp"
#include "ViewListSmsState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    ViewSingleSmsState::ViewSingleSmsState(Context &context, std::size_t smsIndex)
        : BaseState(context, "ViewSingleSmsState"), smsIndex(smsIndex)
    {
        const auto &allSms = context.smsStorage.getAllMessages();
        if (smsIndex >= allSms.size())
        {
            logger.logError("Invalid SMS index provided: ", smsIndex);
            context.setState<ViewListSmsState>();
            return;
        }
        context.smsStorage.markAsRead(smsIndex);
        context.user.showSmsView(allSms[smsIndex]);
    }

    void ViewSingleSmsState::handleUserBack()
    {
        context.setState<ViewListSmsState>();
    }

    void ViewSingleSmsState::handleUserAction(std::optional<std::size_t> /*selectedIndex*/)
    {
        context.setState<ViewListSmsState>();
    }

    void ViewSingleSmsState::handleSmsReceive(common::PhoneNumber sender, std::string text)
    {
        logger.logInfo("SMS received (from: ", sender, ") while viewing message.");
        context.smsStorage.addMessage(sender, text);
        context.user.showNewMessage();
    }

    void ViewSingleSmsState::handleDisconnected()
    {
        context.setState<NotConnectedState>();
    }


}
