#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUeGuiMock.hpp"
#include "Mocks/IEventsHandlerMock.hpp"

namespace ue
{
using namespace ::testing;

class UserPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IEventsHandlerMock> handlerMock;
    StrictMock<IUeGuiMock> guiMock;
    StrictMock<IListViewModeMock> listViewModeMock;
    StrictMock<ITextModeMock> textModeMock;
    StrictMock<IDialModeMock> dialModeMock;
    StrictMock<ICallModeMock> callModeMock;
    StrictMock<ITextModeMock> alertModeMock;
    StrictMock<ISmsComposeModeMock> smsComposeModeMock;

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite(){
        EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER))));
        EXPECT_CALL(guiMock, setAcceptCallback(_));
        EXPECT_CALL(guiMock, setRejectCallback(_));
        objectUnderTest.start(handlerMock);
    }

    ~UserPortTestSuite() override {
        EXPECT_CALL(guiMock, setAcceptCallback(IsNull()));
        EXPECT_CALL(guiMock, setRejectCallback(IsNull()));
        objectUnderTest.stop();
    }
};

TEST_F(UserPortTestSuite, shallStartStop){
    SUCCEED();
}

TEST_F(UserPortTestSuite, shallShowNotConnected){
    EXPECT_CALL(guiMock, showNotConnected());
    objectUnderTest.showNotConnected();
}

TEST_F(UserPortTestSuite, shallShowConnecting){
    EXPECT_CALL(guiMock, showConnecting());
    objectUnderTest.showConnecting();
}

TEST_F(UserPortTestSuite, shallShowMenuOnConnected){
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Compose SMS", ""));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("View SMS", ""));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Call", ""));
    EXPECT_CALL(guiMock, showConnected());
    objectUnderTest.showConnected();
}

TEST_F(UserPortTestSuite, shallShowNewMessage){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("[USER-PORT]New Message has arrived")));
    EXPECT_CALL(guiMock, showNewSms(true));
    objectUnderTest.showNewMessage();
}

TEST_F(UserPortTestSuite, shallShowNewMessageReceived){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("[USER-PORT]New Message has been received")));
    EXPECT_CALL(guiMock, showNewSms(false));
    objectUnderTest.showNewMessageReceived();
}

TEST_F(UserPortTestSuite, shallShowListMessage){
    std::vector<SmsMessage> messages;
    messages.emplace_back(PHONE_NUMBER, "Hello", SmsMessage::Status::receiveR);
    messages.emplace_back(PHONE_NUMBER, "Fail message", SmsMessage::Status::failed);
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("[USER-PORT]Showing SMS List (Count: " + std::to_string(messages.size()) + ")")));
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem("  To: 112", "Hello"));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("  To: 112 [FAILED]", "Fail message"));
    objectUnderTest.showListMessage(messages);
}

TEST_F(UserPortTestSuite, shallShowMessageView){
    SmsMessage message(PHONE_NUMBER, "Test message", SmsMessage::Status::receiveR);
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("[USER-PORT]New Message has been received")));
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::AnyOf(
        testing::HasSubstr("[USER-PORT]From: " + common::to_string(message.sender)),
        testing::HasSubstr("[USER-PORT]To: " + common::to_string(message.sender))
    )));
    EXPECT_CALL(guiMock, showNewSms(false));
    EXPECT_CALL(guiMock, setViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(testing::AnyOf(
        HasSubstr("From: 112"),
        HasSubstr("To: 112")
    )));
    EXPECT_CALL(loggerMock, log(common::ILogger::DEBUG_LEVEL, testing::HasSubstr("Wyświetlam wiadomość: ")));
    objectUnderTest.showMessageView(message);
}

TEST_F(UserPortTestSuite, shallShowNotify){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing Alert: Title")));
    EXPECT_CALL(guiMock, setAlertMode()).WillOnce(ReturnRef(alertModeMock));
    EXPECT_CALL(alertModeMock, setText("Title\n\nMessage"));
    objectUnderTest.showNotify("Title", "Message");
}

TEST_F(UserPortTestSuite, shallShowMessageComp){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing SMS Compose screen")));
    EXPECT_CALL(guiMock, setSmsComposeMode()).WillOnce(ReturnRef(smsComposeModeMock));
    EXPECT_CALL(smsComposeModeMock, clearSmsText());
    objectUnderTest.showMessageComp();
}

TEST_F(UserPortTestSuite, shallShowCallComp){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing Call screen")));
    EXPECT_CALL(guiMock, setDialMode()).WillOnce(ReturnRef(dialModeMock));
    objectUnderTest.showCallComp();
}

TEST_F(UserPortTestSuite, shallShowIncomingCall){
    common::PhoneNumber caller{123};
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing incoming call from: " + common::to_string(caller))));
    EXPECT_CALL(guiMock, setAlertMode()).WillOnce(ReturnRef(alertModeMock));
    EXPECT_CALL(alertModeMock, setText(HasSubstr("INCOMING CALL")));
    objectUnderTest.showIncomingCall(caller);
}

TEST_F(UserPortTestSuite, shallShowCallTalkInterface){
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Show call talk gui")));
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    objectUnderTest.showCallTalkInterface();
}

TEST_F(UserPortTestSuite, shallShowCallInProgress){
    common::PhoneNumber other{123};
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing call in progress with: " + common::to_string(other))));
    EXPECT_CALL(guiMock, setAlertMode()).WillOnce(ReturnRef(alertModeMock));
    EXPECT_CALL(alertModeMock, setText(HasSubstr("CALLING")));
    objectUnderTest.showCallInProgress(other);
}

TEST_F(UserPortTestSuite, shallShowEndedCall){
    common::PhoneNumber other{123};
    std::string reason = "reason";
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing ended call with: " + common::to_string(other) + " reason: " + reason)));
    objectUnderTest.showEndedCall(other, reason);
}

TEST_F(UserPortTestSuite, shallShowCallFailed){
    common::PhoneNumber other{123};
    std::string error = "error";
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("Showing failed call with: " + common::to_string(other) + " error: " + error)));
    objectUnderTest.showCallFailed(other, error);
}

TEST_F(UserPortTestSuite, shallShowAlertPeerUnknownRecipient){
    common::PhoneNumber other{123};
    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, testing::HasSubstr("BTS does not recognize this phone number")));
    EXPECT_CALL(guiMock, setAlertMode()).WillOnce(ReturnRef(alertModeMock));
    EXPECT_CALL(alertModeMock, setText(HasSubstr("Unknown peer")));
    objectUnderTest.showAlertPeerUnknownRecipient(other);
}

TEST_F(UserPortTestSuite, shallShowCallMenu){
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Dial Number", "Enter a number to call"));
    objectUnderTest.showCallMenu();
}


TEST_F(UserPortTestSuite, shallGetMessageRecipient){
    EXPECT_CALL(guiMock, setSmsComposeMode()).WillOnce(ReturnRef(smsComposeModeMock));
    EXPECT_CALL(smsComposeModeMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_EQ(objectUnderTest.getMessageRecipient(), PHONE_NUMBER);
}

TEST_F(UserPortTestSuite, shallGetMessageText){
    EXPECT_CALL(guiMock, setSmsComposeMode()).WillOnce(ReturnRef(smsComposeModeMock));
    EXPECT_CALL(smsComposeModeMock, getSmsText()).WillOnce(Return("text"));
    EXPECT_EQ(objectUnderTest.getMessageText(), "text");
}

TEST_F(UserPortTestSuite, shallGetCallRecipient){
    EXPECT_CALL(guiMock, setDialMode()).WillOnce(ReturnRef(dialModeMock));
    EXPECT_CALL(dialModeMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_EQ(objectUnderTest.getCallRecipient(), PHONE_NUMBER);
}

TEST_F(UserPortTestSuite, shallGetCallText){
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return("call text"));
    EXPECT_EQ(objectUnderTest.getCallText(), "call text");
}

TEST_F(UserPortTestSuite, shallClearIncomingCallText){
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, clearIncomingText());
    objectUnderTest.clearIncomingCallText();
}

TEST_F(UserPortTestSuite, shallClearOutgoingCallText){
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, clearOutgoingText());
    objectUnderTest.clearOutgoingCallText();
}

TEST_F(UserPortTestSuite, shallAppendCallText){
    std::string message = "append message";
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, appendIncomingText(message));
    objectUnderTest.appendCallText(message);
}

}
