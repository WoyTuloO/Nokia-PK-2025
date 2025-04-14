#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include <optional>

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IUserEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
}

void UserPort::stop()
{
    handler = nullptr;
}

void UserPort::showNotConnected()
{
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    gui.showConnecting();
}

void UserPort::showConnected()
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "Send a new message");
    menu.addSelectionListItem("View SMS", "View received messages");
}

void UserPort::showNewMessage()
{
    logger.logInfo("New message received");
    gui.showNewSms(true);
}

void UserPort::showSmsList(const std::vector<SmsMessage>& messages)
{
    IUeGui::IListViewMode &listMode = gui.setListViewMode();
    listMode.clearSelectionList();
    for (const auto &msg : messages)
    {
        std::string prefix = (msg.isRead ? "  " : "* ");
        std::string label = prefix + "From: " + to_string(msg.sender);
        listMode.addSelectionListItem(label, msg.text);
    }
}

void UserPort::showSmsView(const SmsMessage &message)
{
    IUeGui::ITextMode &textMode = gui.setViewTextMode();
    std::string displayText = "From: " + to_string(message.sender) + "\n\n" + message.text;
    textMode.setText(displayText);
}

void UserPort::showSmsCompose()
{
    IUeGui::ITextMode &composeMode = gui.setSmsComposeMode();
    composeMode.clearSmsText();
    composeMode.setPhoneNumber(common::PhoneNumber{});
}

void UserPort::showAlert(const std::string &title, const std::string &message)
{
    IUeGui::ITextMode &alertMode = gui.setAlertMode();
    alertMode.setText(title + "\n\n" + message);
}

common::PhoneNumber UserPort::getSmsRecipient() const
{
    return gui.getSmsComposeMode().getPhoneNumber();
}

std::string UserPort::getSmsText() const
{
    return gui.getSmsComposeMode().getSmsText();
}

}
