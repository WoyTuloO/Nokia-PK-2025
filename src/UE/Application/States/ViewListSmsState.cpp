#include "ViewListSmsState.hpp"
#include "ConnectedState.hpp"
#include "IncomingCallState.hpp"
#include "NotConnectedState.hpp"
#include "ViewSmsState.hpp"

namespace ue
{

ViewListSmsState::ViewListSmsState(Context& ctx) : BaseState(ctx, "ViewListSmsState")
{
    logger.logDebug("Entering SMS List view");
    refreshList();
}

void ViewListSmsState::refreshList()
{
    messages_ = context.smsStorage.getAllMessages();

    if (messages_.empty())
    {
        logger.logInfo("SMS list is empty");
        context.user.showNotify("SMS Inbox", "No messages.");
        context.setState<ConnectedState>();
        return;
    }

    logSmsListDebug();
    context.user.showListMessage(messages_);
}

void ViewListSmsState::logSmsListDebug()
{
    logger.logDebug("Displaying SMS list (count=", messages_.size(), ")");
    for (std::size_t i = 0; i < messages_.size(); ++i)
    {
        const auto& msg = messages_[i];
        logger.logDebug("  SMS #", i, " from=", common::to_string(msg.sender), " read=", isRead(msg) ? "yes" : "no");
    }
}

bool ViewListSmsState::isRead(const SmsMessage& msg) const
{
    if (msg.direction != SmsMessage::Dir::in)
        return true;
    return msg.status == SmsMessage::Status::receiveUR;
}

void ViewListSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    if (!selectedIndex.has_value())
    {
        logger.logInfo("UI Action with no index in SMS List");
        return;
    }

    auto idx = *selectedIndex;
    if (idx >= messages_.size())
    {
        logger.logError("Selected index out of bounds: ", idx, " (list size=", messages_.size(), ")");
        return;
    }

    logger.logInfo("SMS selected at index=", idx);
    context.setState<ViewSmsState>(idx);
}

void ViewListSmsState::handleUiBack()
{
    logger.logInfo("Back action from SMS List view");
    context.setState<ConnectedState>();
}

void ViewListSmsState::handleDisconnected()
{
    logger.logInfo("Connection lost while viewing SMS list");
    context.setState<NotConnectedState>();
}

void ViewListSmsState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    logger.logInfo("SMS received while in list (from=", from, ")");
    auto newIndex = context.smsStorage.addMessage(from, text);
    logger.logDebug("Stored new SMS at index=", newIndex);
    context.user.showNewMessage();
    refreshList();
}

void ViewListSmsState::handleCallRequest(common::PhoneNumber from)
{
    logger.logDebug("4.2.4.4 UE receives Call Request, while viewing SMS/SMS list");
    logger.logInfo("Incoming call - overriding and switching to IncomingCallState");

    context.setState<IncomingCallState>(from);
}

}
