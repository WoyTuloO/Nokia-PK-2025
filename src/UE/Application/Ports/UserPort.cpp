#include "UserPort.hpp"
#include "Traits/EnumTraits.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include "UeGui/IDialMode.hpp"
#include "Utils/todo.h"

#include <format>
#include <string>

namespace ue
{
using view_mode = view_details::GuiViewMode;

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"), gui(gui), phoneNumber(phoneNumber)
{}

void UserPort::start(IEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
    gui.setAcceptCallback(std::bind(&UserPort::acceptCallback, this));
    gui.setRejectCallback(std::bind(&UserPort::rejectCallback, this));
}

void UserPort::stop()
{
    handler = nullptr;

    gui.setAcceptCallback(nullptr);
    gui.setRejectCallback(nullptr);
}

void UserPort::showNotConnected()
{
    currentViewMode = view_mode::Default;
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    currentViewMode = view_mode::Default;
    gui.showConnecting();
}

void UserPort::showConnected()
{
    currentViewMode = view_mode::Menu;
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");
    menu.addSelectionListItem("Call", "");

    gui.showConnected();
}

void UserPort::showNewMessage()
{
    logger.logInfo("New Message has arrived");
    gui.showNewSms(true);
}

void UserPort::showNewMessageReceived()
{
    logger.logInfo("New Message has been received");
    gui.showNewSms(false);
}

void UserPort::showListMessage(const std::vector<SmsMessage> &messages)
{
    currentViewMode = view_mode::Message_list;
    logger.logInfo("Showing SMS List (Count: ", messages.size(), ")");
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();

    for (const auto &sms: messages)
    {
        bool isRead = (sms.direction == SmsMessage::Dir::in) ? (sms.status == SmsMessage::Status::receiveR) : true;

        std::string prefix = isRead ? "  " : "* ";

        std::string directionLabel = (sms.direction == SmsMessage::Dir::in) ? "From: " : "To: ";

        std::string label = prefix + directionLabel + common::to_string(sms.sender);

        if (sms.direction == SmsMessage::Dir::out && sms.status == SmsMessage::Status::failed)
        {
            label += " [FAILED]";
        }

        menu.addSelectionListItem(label, sms.text);
    }
}

void UserPort::showMessageView(const SmsMessage &message)
{
    currentViewMode = view_mode::Message_view;

    std::string labelPrefix = (message.direction == SmsMessage::Dir::in) ? "From: " : "To: ";
    logger.logInfo(labelPrefix, message.sender);

    IUeGui::ITextMode &viewer = gui.setViewTextMode();

    std::string displayText = labelPrefix + common::to_string(message.sender);
    displayText += "\n\n--- Treść wiadomości ---\n";
    displayText += message.text;

    if (message.direction == SmsMessage::Dir::out)
    {
        std::string statusText = (message.status == SmsMessage::Status::failed) ? " [FAILED]" : "";
        displayText += statusText;
    }
    logger.logDebug("Wyświetlam wiadomość: ", displayText);
    viewer.setText(displayText);
    showNewMessageReceived();
}

void UserPort::showNotify(const std::string &title, const std::string &message)
{
    currentViewMode = view_mode::Default;
    logger.logInfo("Showing Alert: ", title);
    IUeGui::ITextMode &alerter = gui.setAlertMode();
    alerter.setText(title + "\n\n" + message);
}

void UserPort::showMessageComp()
{
    currentViewMode = view_mode::Message_compose;
    logger.logInfo("Showing SMS Compose screen");
    auto &composeMode = gui.setSmsComposeMode();

    composeMode.clearSmsText();
}

void UserPort::showCallComp()
{
    currentViewMode = view_mode::Call_compose;
    logger.logInfo("Showing Call screen");
    this->gui.setDialMode();
}

void UserPort::showIncomingCall(const common::PhoneNumber &caller)
{
    currentViewMode = view_mode::Call_incoming;
    logger.logInfo("Showing incoming call from: ", caller);
    auto &mode = this->gui.setAlertMode();

    mode.setText(std::format("INCOMING CALL\n[{:0>3d}]", caller.value));
}

void UserPort::showCallTalkInterface()
{
    currentViewMode = view_mode::Call_talk;
    logger.logInfo("Show call talk gui");
    this->gui.setCallMode();
}

void UserPort::showCallInProgress(const common::PhoneNumber &otherPhoneNumber)
{
    currentViewMode = view_mode::Call_outgoing;
    logger.logInfo("Showing call in progress with: ", otherPhoneNumber);
    auto &mode = this->gui.setAlertMode();

    mode.setText(std::format("CALLING\n[{:0>3d}]", otherPhoneNumber.value));
}

void UserPort::showEndedCall(const common::PhoneNumber &otherPhoneNumber, const std::string &reason)
{
    logger.logInfo("Showing ended call with: ", otherPhoneNumber, " reason: ", reason);
}

void UserPort::showCallFailed(const common::PhoneNumber &otherPhoneNumber, const std::string &errorMessage)
{
    logger.logInfo("Showing failed call with: ", otherPhoneNumber, " error: ", errorMessage);
}

void UserPort::showAlertPeerUnknownRecipient(const common::PhoneNumber& otherPhoneNumber)
{
    currentViewMode = view_mode::Call_alert_UR;
    logger.logInfo(std::format("BTS does not recognize this phone number: {:0>3d}", otherPhoneNumber.value));

    auto& mode = this->gui.setAlertMode();

    mode.setText(std::format("Unknown peer\n"
                             "[{:0>3d}]\n"
                             "\nPeer you tried"
                             "\nto call"
                             "\nprobably"
                             "\ndisconnected", otherPhoneNumber.value));
}

void UserPort::showCallMenu()
{
    currentViewMode = view_mode::Call_menu;
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Dial Number", "Enter a number to call");
}

void UserPort::acceptCallback()
{
    if (!handler)
        return;

    std::optional<std::size_t> selectedIndexOpt;

    switch (currentViewMode)
    {
    case view_mode::Menu:;
        {
            logger.logDebug("Accept in main menu - getting selected index");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
        }
        break;
    case view_mode::Message_menu:;
        {
            logger.logDebug("Accept in SMS menu - getting selected index");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;

            if (selectedIndexOpt.has_value())
            {
                if (selectedIndexOpt.value() == 0)
                {
                    logger.logInfo("Compose SMS selected from SMS menu");
                    showMessageComp();
                    selectedIndexOpt = std::nullopt;
                }
            }
        }
        break;
    case view_mode::Message_list:;
        {
            logger.logDebug("Accept in SMS list - getting selected SMS");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
        }
        break;
    case view_mode::Message_compose:;
        {
            logger.logDebug("Accept in SMS compose - sending message");
            selectedIndexOpt = std::nullopt;
        }
        break;
    case view_mode::Call_compose:;
        {
            logger.logDebug("Accept in SMS list - dialling");
            selectedIndexOpt = std::nullopt;
        }
        break;
    case view_mode::Call_outgoing:;
        {
            logger.logDebug("Accept from caller when calling - does nothing");
            selectedIndexOpt = std::nullopt;
        }
        break;
    case view_mode::Call_incoming:;
        {
            logger.logDebug("Accept from callee when being called - Accept Call");
            selectedIndexOpt = std::nullopt;
        }
        break;
    case view_mode::Call_talk:;
        {
            logger.logDebug("Send message to other caller - Send message");
            selectedIndexOpt = std::nullopt;
        }
        break;
    case view_mode::Call_alert_UR:;
        {
            logger.logDebug("Tried connecting to peer BTS couldn't find");
            selectedIndexOpt = 1UL;
        }
        break;
    // Deprecated modes
    case view_mode::Message_view:;
        [[fallthrough]];
    case view_mode::Call_menu:;
        [[fallthrough]];
    default:;
        {
            logger.logError("Aplication set to undefined view mode");
            selectedIndexOpt = std::nullopt;
        }
        break;
    }

    logger.logDebug("Sending UI action to handler, mode: ", common::enumUnderlyingValue(currentViewMode));
    handler->handleUiAction(selectedIndexOpt);
}

void UserPort::rejectCallback()
{
    if (!handler)
        return;
    logger.logDebug("UI Action (Reject/Back), Mode: ", common::enumUnderlyingValue(currentViewMode));

    handler->handleUiBack();
}

common::PhoneNumber UserPort::getMessageRecipient() const
{
    return gui.setSmsComposeMode().getPhoneNumber();
}

std::string UserPort::getMessageText() const
{
    return gui.setSmsComposeMode().getSmsText();
}

common::PhoneNumber UserPort::getCallRecipient() const
{
    return gui.setDialMode().getPhoneNumber();
}

std::string UserPort::getCallText() const
{
    return gui.setCallMode().getOutgoingText();
}

void UserPort::clearIncomingCallText()
{
    gui.setCallMode().clearIncomingText();
}

void UserPort::clearOutgoingCallText()
{
    gui.setCallMode().clearOutgoingText();
}

void UserPort::appendCallText(std::string const& message)
{
    gui.setCallMode().appendIncomingText(message);
}


}