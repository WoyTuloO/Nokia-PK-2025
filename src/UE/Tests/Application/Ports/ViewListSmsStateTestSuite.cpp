#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "States/ViewListSmsState.hpp"
#include "SmsStorage.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Context.hpp"

using namespace ::testing;
using namespace ue;

class ViewListSmsStateTestSuite : public ::testing::Test {
protected:
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsMock;
    StrictMock<IUserPortMock> userMock;
    StrictMock<ITimerPortMock> timerMock;
    SmsStorage smsStorage;
    common::PhoneNumber myPhoneNumber{112};

    Context makeContext() {
        return Context{
            loggerMock,
            btsMock,
            userMock,
            timerMock,
            {},
            smsStorage,
            myPhoneNumber
        };
    }
};

TEST_F(ViewListSmsStateTestSuite, ShowsNotifyIfEmpty) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showNotify(_, _)).Times(1);
    EXPECT_CALL(userMock, showConnected()).Times(AnyNumber());
    ViewListSmsState state(ctx);
}

TEST_F(ViewListSmsStateTestSuite, ShowsListIfNotEmpty) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{1}, "msg");
    EXPECT_CALL(userMock, showListMessage(_)).Times(1);
    ViewListSmsState state(ctx);
}

TEST_F(ViewListSmsStateTestSuite, handleMessageReceive_AddsMessageAndRefreshesList) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{1}, "old");

    EXPECT_CALL(userMock, showListMessage(_)).Times(2);
    EXPECT_CALL(userMock, showNewMessage()).Times(1);

    ViewListSmsState state(ctx);
    state.handleMessageReceive(common::PhoneNumber{222}, "nowy");

    EXPECT_EQ(ctx.smsStorage.getAllMessages().back().text, "nowy");
}

TEST_F(ViewListSmsStateTestSuite, handleUiBack_CallsShowListMessage) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{1}, "foo");

    EXPECT_CALL(userMock, showListMessage(_)).Times(AnyNumber());
    EXPECT_CALL(userMock, showConnected()).Times(AnyNumber());
    ViewListSmsState state(ctx);
    state.handleUiBack();
}
