#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
#include <string>

namespace ue {

class IBtsEventsHandler {
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
    virtual void handleDisconnected() = 0;
    virtual void handleMessageReceive(common::PhoneNumber from, std::string text) = 0;
    virtual void handleMessageSentResult(common::PhoneNumber to, bool success) = 0;
    virtual void handleCallRequest(common::PhoneNumber from) = 0;
};

class IBtsPort {
public:
    virtual ~IBtsPort() = default;

    virtual void sendAttachRequest(common::BtsId) = 0;
    virtual void sendMessage(common::PhoneNumber to, const std::string& text) = 0;
};

}
