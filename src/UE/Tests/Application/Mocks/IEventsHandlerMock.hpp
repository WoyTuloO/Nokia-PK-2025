#pragma once

#include "Application/IEventsHandler.hpp"
#include <optional>
#include <gmock/gmock.h>

namespace ue{

class IEventsHandlerMock : public IEventsHandler{

    public:

    MOCK_METHOD(void, handleSib, (common::BtsId), (override));
    MOCK_METHOD(void, handleAttachAccept, (), (override));
    MOCK_METHOD(void, handleAttachReject, (), (override));
    MOCK_METHOD(void, handleDisconnected, (), (override));
    MOCK_METHOD(void, handleMessageReceive, (common::PhoneNumber sender, std::string text), (override));

    MOCK_METHOD(void, handleUiBack, (), (override));
    MOCK_METHOD(void, handleUiAction, (std::optional<std::size_t> selectedIdx), (override));

    MOCK_METHOD(void, handleTimeout, (), (override));


};

}




