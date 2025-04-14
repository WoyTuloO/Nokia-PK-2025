#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"

#include <string>
#include "UeGui/ITextMode.hpp"

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IEventsHandler &handler){
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
    gui.setAcceptCallback(std::bind(&UserPort::acceptCallback, this));
    gui.setRejectCallback(std::bind(&UserPort::rejectCallback, this));

}

void UserPort::stop(){
    handler = nullptr;
    gui.setRejectCallback(nullptr);
    gui.setAcceptCallback(nullptr);
}

void UserPort::showNotConnected(){
    currentViewMode = view_details::VM_DEFAULT;
    gui.showNotConnected();
}

void UserPort::showConnecting(){
    currentViewMode = view_details::VM_DEFAULT;
    gui.showConnecting();
}

void UserPort::showConnected(){
    currentViewMode = view_details::VM_MENU;

    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "Send new message");
    menu.addSelectionListItem("View SMS", "Read received message");
    gui.showConnected();
}

void UserPort::showNewMessage(){
    logger.logInfo("New Message has arrived");
    gui.showNewSms(true);

}


void UserPort::showMessageList(const std::vector<SmsMessage>& messages){

        currentViewMode = view_details::VM_MESSAGE_LIST;

        IUeGui::IListViewMode& menu = gui.setListViewMode();

        menu.clearSelectionList();

        for(const auto& mess : messages){
            std::string pref = "  ";
            if(mess.isRead)
                pref = "* ";

            menu.addSelectionListItem(pref + "Sender: " + common::to_string(mess.sender),mess.text);


        }


}

void UserPort:: showMessageView(const SmsMessage& message){
        currentViewMode = view_details::VM_MESSAGE_VIEW;
        IUeGui::ITextMode& view = gui.setViewTextMode();
        view.setText("From: " + common::to_string(message.sender) + "\n\n" + message.text);
}


void UserPort:: showAlert(const std::string& title, const std::string& text){
    currentViewMode = view_details::VM_DEFAULT;

    IUeGui::ITextMode& alert = gui.setAlertMode();
    alert.setText("New Message: " + title + "\n\n" + text);
}


void UserPort:: rejectCallback(){
    if(!handler) return;

    handler->handleUiBack();
}

void UserPort:: acceptCallback(){
    if(!handler) return;
    std::optional<std::size_t> selectedIdx;

    if(currentViewMode == view_details::VM_MESSAGE_VIEW){
        return;
    }else if( currentViewMode == view_details::VM_MENU || currentViewMode == view_details::VM_MESSAGE_LIST){
        IUeGui::IListViewMode& currentList = gui.setListViewMode();
        auto item = gui.setListViewMode().getCurrentItemIndex();
        if(item.first)
            selectedIdx = item.second;
    }

    handler->handleUiAction(selectedIdx);

}



}
