#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "States/ViewSmsState.hpp"
#include "SmsStorage.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Context.hpp"

using namespace ::testing;
using namespace ue;

class ViewSmsStateTestSuite : public ::testing::Test {
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

TEST_F(ViewSmsStateTestSuite, ShowsCurrentSmsOnEntry) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{123}, "siema!");

    EXPECT_CALL(userMock, showMessageView(_)).Times(1);

    ViewSmsState state(ctx, 0);
}

TEST_F(ViewSmsStateTestSuite, MarksAsReadIfReceivedUnread) {
    auto ctx = makeContext();
    auto idx = ctx.smsStorage.addMessage(common::PhoneNumber{123}, "nieprzeczytany");
    EXPECT_CALL(userMock, showMessageView(_)).Times(1);

    ViewSmsState state(ctx, idx);

    EXPECT_EQ(ctx.smsStorage.getAllMessages()[idx].status, SmsMessage::Status::receiveR);
}

TEST_F(ViewSmsStateTestSuite, ReturnToSmsListOnInvalidIndex) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showListMessage(_)).Times(1);
    EXPECT_CALL(userMock, showNotify(_, _)).Times(AnyNumber());
    EXPECT_CALL(userMock, showConnected()).Times(AnyNumber());
    ViewSmsState state(ctx, 42);
}

TEST_F(ViewSmsStateTestSuite, handleMessageReceive_AddsSmsAndShowsNewMessage) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{1}, "old");

    EXPECT_CALL(userMock, showMessageView(_));
    EXPECT_CALL(userMock, showNewMessage());

    ViewSmsState state(ctx, 0);

    auto prevSize = ctx.smsStorage.getAllMessages().size();
    state.handleMessageReceive(common::PhoneNumber{222}, "nowy");

    ASSERT_EQ(ctx.smsStorage.getAllMessages().size(), prevSize + 1);
    EXPECT_EQ(ctx.smsStorage.getAllMessages().back().text, "nowy");
}

TEST_F(ViewSmsStateTestSuite, handleUiBack_ShowsList) {
    auto ctx = makeContext();
    ctx.smsStorage.addMessage(common::PhoneNumber{123}, "aaa");

    EXPECT_CALL(userMock, showMessageView(_));
    EXPECT_CALL(userMock, showListMessage(_)).Times(AtLeast(1));
    EXPECT_CALL(userMock, showConnected()).Times(AnyNumber());
    ViewSmsState state(ctx, 0);
    state.handleUiBack();
}
