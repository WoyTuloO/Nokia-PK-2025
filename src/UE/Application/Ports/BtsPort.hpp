#pragma once

#include "IBtsPort.hpp"
#include "ITransport.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"

// Disturbing comment

namespace ue
{

class BtsPort : public IBtsPort
{
public:
    BtsPort(common::ILogger& logger, common::ITransport& transport, common::PhoneNumber phoneNumber);
    void start(IBtsEventsHandler& handler);
    void stop();

    void sendAttachRequest(common::BtsId) override;
    void sendMessage(common::PhoneNumber to, const std::string& text) override;
    void sendCallRequest(common::PhoneNumber to) override;
    void sendCallDropped(common::PhoneNumber to) override;
    void sendCallTalk(common::PhoneNumber to, const std::string& text) override;
    void sendCallAccepted(common::PhoneNumber to) override;
    void sendUnknownRecipient(common::PhoneNumber to) override;

private:
    void handleMessage(BinaryMessage msg);
    void handleDisconnected();

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber;

    IBtsEventsHandler* handler = nullptr;
};

}
