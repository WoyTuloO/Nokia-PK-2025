//
// Created by student on 5/26/25.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "States/ComposeSmsState.hpp"
#include "SmsStorage.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Context.hpp"

using namespace ::testing;
using namespace ue;

class ComposeSmsStateTestSuite : public ::testing::Test {
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

TEST_F(ComposeSmsStateTestSuite, ShowsMessageCompOnEntry) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showMessageComp()).Times(1);
    ComposeSmsState state(ctx);
}

TEST_F(ComposeSmsStateTestSuite, handleUiAction_InvalidInputsShowsNotify) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showMessageComp()).Times(1);
    EXPECT_CALL(userMock, getMessageRecipient()).WillOnce(Return(common::PhoneNumber{}));
    EXPECT_CALL(userMock, getMessageText()).WillOnce(Return(""));
    EXPECT_CALL(userMock, showNotify(_, _)).Times(1);
    ComposeSmsState state(ctx);
    state.handleUiAction(0);
}

TEST_F(ComposeSmsStateTestSuite, handleUiAction_ValidInputsSendsSms) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showMessageComp()).Times(AnyNumber());
    EXPECT_CALL(userMock, getMessageRecipient()).WillOnce(Return(common::PhoneNumber{123}));
    EXPECT_CALL(userMock, getMessageText()).WillOnce(Return("text"));
    EXPECT_CALL(btsMock, sendMessage(common::PhoneNumber{123}, "text")).Times(1);

    EXPECT_CALL(userMock, showConnected()).Times(AnyNumber());
    EXPECT_CALL(userMock, showNotify(_, _)).Times(AnyNumber());
    EXPECT_CALL(userMock, showListMessage(_)).Times(AnyNumber());
    ComposeSmsState state(ctx);
    state.handleUiAction(0);
    EXPECT_EQ(ctx.smsStorage.getAllMessages().back().text, "text");
    EXPECT_EQ(ctx.smsStorage.getAllMessages().back().sender, common::PhoneNumber{123});
}

TEST_F(ComposeSmsStateTestSuite, handleMessageReceive_AddsMessageAndShowsNew) {
    auto ctx = makeContext();
    EXPECT_CALL(userMock, showMessageComp()).Times(1);
    EXPECT_CALL(userMock, showNewMessage()).Times(1);
    ComposeSmsState state(ctx);
    auto prevSize = ctx.smsStorage.getAllMessages().size();
    state.handleMessageReceive(common::PhoneNumber{100}, "hej!");
    EXPECT_EQ(ctx.smsStorage.getAllMessages().size(), prevSize + 1);
    EXPECT_EQ(ctx.smsStorage.getAllMessages().back().text, "hej!");
}
