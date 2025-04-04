#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <memory>

namespace ue
{
using namespace ::testing;

class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::BtsId BTS_ID{1001};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;

    Expectation showNotConnected = EXPECT_CALL(userPortMock, showNotConnected());

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{
    void sendAttachRequestOnSib(){
        using namespace std::chrono_literals;
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
        EXPECT_CALL(timerPortMock, startTimer(500ms));
        EXPECT_CALL(userPortMock, showConnecting());
        objectUnderTest.handleSib(BTS_ID);
    }
};

TEST_F(ApplicationNotConnectedTestSuite, shallSendAttachRequestOnSib)
{
    sendAttachRequestOnSib();
}


struct ApplicationConnectingTestSuite : ApplicationNotConnectedTestSuite{
    ApplicationConnectingTestSuite(){
        sendAttachRequestOnSib();
    }

};

TEST_F(ApplicationConnectingTestSuite, shallConnectOnAttachAccept){
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.handleAttachAccept();
}


TEST_F(ApplicationConnectingTestSuite, shallDisconnectOnAttachReject){
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleAttachReject();
}

TEST_F(ApplicationConnectingTestSuite, shallDisconnectOnTimeout){
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleTimeout();
}


struct ApplicationConnectedTestSuite : ApplicationNotConnectedTestSuite{
    ApplicationConnectedTestSuite();
};

ApplicationConnectedTestSuite::ApplicationConnectedTestSuite(){
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.handleAttachAccept();

}

}
