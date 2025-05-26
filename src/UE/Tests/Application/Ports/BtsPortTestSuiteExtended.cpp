#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Messages/IncomingMessage.hpp"
#include "Messages/MessageId.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/ITransportMock.hpp"
#include "Ports/BtsPort.hpp"

namespace ue
{

using namespace ::testing;

class BtsPortTestSuiteExtended : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{ 112 };
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsEventsHandlerMock> handlerMock;
    NiceMock<common::ITransportMock> transportMock;
    common::ITransport::MessageCallback messageCallback;
    common::ITransport::DisconnectedCallback disconnectedCallback;
    common::BinaryMessage msg;

    BtsPort objectUnderTest{ loggerMock, transportMock, PHONE_NUMBER };

    void SetUp() override
    {
        ON_CALL(transportMock, registerMessageCallback(_)).WillByDefault(SaveArg<0>(&messageCallback));
        ON_CALL(transportMock, registerDisconnectedCallback(_)).WillByDefault(SaveArg<0>(&disconnectedCallback));
        ON_CALL(transportMock, sendMessage(_)).WillByDefault(DoAll(SaveArg<0>(&msg), Return(true)));

        objectUnderTest.start(handlerMock);
    }

    void TearDown() override
    {
        objectUnderTest.stop();
    }

    common::BinaryMessage createMessage(common::MessageId id)
    {
        common::OutgoingMessage msg{ id, common::PhoneNumber{}, PHONE_NUMBER };
        return msg.getMessage();
    }

    common::BinaryMessage createCallRequestMessage(common::PhoneNumber from)
    {
        common::OutgoingMessage msg{ common::MessageId::CallRequest, from, PHONE_NUMBER };
        return msg.getMessage();
    }

    common::BinaryMessage createCallTalkMessage(common::PhoneNumber from, const std::string& text)
    {
        common::OutgoingMessage msg{ common::MessageId::CallTalk, from, PHONE_NUMBER };
        msg.writeText(text);
        return msg.getMessage();
    }
};

TEST_F(BtsPortTestSuiteExtended, shallSendCallRequest)
{
    EXPECT_CALL(transportMock, sendMessage(_));

    objectUnderTest.sendCallRequest(common::PhoneNumber{ 123 });
    common::IncomingMessage reader(msg);

    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallRequest, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{123}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuiteExtended, shallSendCallAccepted)
{
    EXPECT_CALL(transportMock, sendMessage(_));

    objectUnderTest.sendCallAccepted(common::PhoneNumber{ 123 });
    common::IncomingMessage reader(msg);

    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallAccepted, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{123}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuiteExtended, shallSendCallDropped)
{
    EXPECT_CALL(transportMock, sendMessage(_));

    objectUnderTest.sendCallDropped(common::PhoneNumber{ 123 });
    common::IncomingMessage reader(msg);

    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallDropped, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{123}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuiteExtended, shallSendCallTalk)
{
    EXPECT_CALL(transportMock, sendMessage(_));

    objectUnderTest.sendCallTalk(common::PhoneNumber{ 123 }, "Hello");
    common::IncomingMessage reader(msg);

    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallTalk, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{123}, reader.readPhoneNumber()));

    std::string text = reader.readRemainingText();
    ASSERT_NO_THROW(EXPECT_EQ("Hello", text));

    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuiteExtended, shallSendUnknownRecipient)
{
    EXPECT_CALL(transportMock, sendMessage(_));

    objectUnderTest.sendUnknownRecipient(common::PhoneNumber{ 123 });
    common::IncomingMessage reader(msg);

    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::UnknownRecipient, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{123}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuiteExtended, shallHandleCallRequest)
{
    EXPECT_CALL(handlerMock, handleCallRequest(common::PhoneNumber{123}));
    messageCallback(createCallRequestMessage(common::PhoneNumber{ 123 }));
}

TEST_F(BtsPortTestSuiteExtended, shallHandleCallAccepted)
{
    common::OutgoingMessage msg{ common::MessageId::CallAccepted, common::PhoneNumber{ 123 }, PHONE_NUMBER };
    common::BinaryMessage binMsg = msg.getMessage();

    EXPECT_CALL(handlerMock, handleCallAccepted(common::PhoneNumber{123}));
    messageCallback(binMsg);
}

TEST_F(BtsPortTestSuiteExtended, shallHandleCallDropped)
{
    common::OutgoingMessage msg{ common::MessageId::CallDropped, common::PhoneNumber{ 123 }, PHONE_NUMBER };
    common::BinaryMessage binMsg = msg.getMessage();

    EXPECT_CALL(handlerMock, handleCallDropped(common::PhoneNumber{123}));
    messageCallback(binMsg);
}

TEST_F(BtsPortTestSuiteExtended, shallHandleCallTalk)
{
    common::OutgoingMessage msg{ common::MessageId::CallTalk, common::PhoneNumber{ 123 }, PHONE_NUMBER };
    msg.writeText("Hello");
    common::BinaryMessage binMsg = msg.getMessage();

    EXPECT_CALL(handlerMock, handleCallTalk(common::PhoneNumber{123}, "Hello"));
    messageCallback(binMsg);
}

TEST_F(BtsPortTestSuiteExtended, shallHandleUnknownRecipient)
{
    common::OutgoingMessage msg{ common::MessageId::UnknownRecipient, common::PhoneNumber{ 123 }, PHONE_NUMBER };
    common::BinaryMessage binMsg = msg.getMessage();

    EXPECT_CALL(handlerMock, handleUnknownRecipient(common::PhoneNumber{123}));
    messageCallback(binMsg);
}

}
