#pragma once

#include "ITimerPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include <thread>
#include <condition_variable>

namespace ue{

class TimerPort : public ITimerPort{
public:
    explicit TimerPort(common::ILogger& logger);

    void start(ITimerEventsHandler& handler);
    void stop();

    void startTimer(Duration duration) override;
    void stopTimer() override;

private:
    common::PrefixedLogger logger;
    ITimerEventsHandler* handler = nullptr;
    std::mutex countdown_guard {};
    std::condition_variable countdown_cv {};

    void startCountdown(Duration duration);
    void stopCountdown();
};

}
