#pragma once
#include "BaseState.hpp"

namespace ue
{
class DiallingState : public BaseState
{
public:
    DiallingState(Context& context);
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
private:
    [[nodiscard]] constexpr inline bool validateCallNumber() const noexcept;
};

class OutgoingDiallingState : public BaseState
{
public:
    OutgoingDiallingState(Context& context, common::PhoneNumber to);
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleTimeout() override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleUnknownRecipient(common::PhoneNumber from) override;
private:
    common::PhoneNumber number_to_call;
};
}