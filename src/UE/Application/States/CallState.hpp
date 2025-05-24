#pragma once
#include "BaseState.hpp"

namespace ue
{
class DiallingState : public BaseState
{
public:
    explicit DiallingState(Context& context);
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;
    void handleCallRequest(common::PhoneNumber from) override;
private:
    [[nodiscard]] constexpr inline bool validateCallNumber() const noexcept;
};

class OutgoingDiallingState : public BaseState
{
public:
    OutgoingDiallingState(Context& context, common::PhoneNumber to);
    ~OutgoingDiallingState();
    OutgoingDiallingState(OutgoingDiallingState const& o) = default;
    OutgoingDiallingState& operator=(OutgoingDiallingState const& o) = default;

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleTimeout() override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleUnknownRecipient(common::PhoneNumber from) override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;
private:
    common::PhoneNumber number_to_call;
};
}