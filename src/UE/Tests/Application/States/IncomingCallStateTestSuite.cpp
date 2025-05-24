#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "States/IncomingCallState.hpp"

namespace ue
{
using namespace ::testing;

class IncomingCallStateTestSuite : public Test
{
protected:
    const common::PhoneNumber caller{ 123 };
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;

    SmsStorage smsStorage;
    Context context;
    std::unique_ptr<IncomingCallState> state;

    IncomingCallStateTestSuite() : context{ loggerMock, btsPortMock, userPortMock, timerPortMock }
    {}

    void SetUp() override
    {
        context.myPhoneNumber = common::PhoneNumber{ 67 };
        state = std::make_unique<IncomingCallState>(context, caller);
    }
};

MATCHER_P(DurationEq, d, "")
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(arg).count()
           == std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

TEST_F(IncomingCallStateTestSuite, UserAcceptsCallWithinTimeout)
{
    EXPECT_CALL(userPortMock, showIncomingCall(caller));
    EXPECT_CALL(timerPortMock, startTimer(DurationEq(std::chrono::seconds(30))));

    state = std::make_unique<IncomingCallState>(context, caller);

    EXPECT_CALL(btsPortMock, sendCallAccepted(caller));
    EXPECT_CALL(timerPortMock, stopTimer());

    state->handleUiAction(std::nullopt);
}

}
