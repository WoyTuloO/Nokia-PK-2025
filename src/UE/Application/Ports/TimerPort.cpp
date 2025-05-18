#include "TimerPort.hpp"
#include "Utils/unreachable.hpp"

namespace ue
{

TimerPort::TimerPort(common::ILogger &logger) : logger(logger, "[TIMER PORT]")
{}

void TimerPort::start(ITimerEventsHandler &handler)
{
    logger.logDebug("Started");
    this->handler = &handler;
}

void TimerPort::stop()
{
    logger.logDebug("Stopped");
    handler = nullptr;
}

void TimerPort::startTimer(Duration duration)
{
    logger.logDebug("Start timer: ", duration.count(), "ms");
    this->startCountdown(duration);
}

void TimerPort::stopTimer()
{
    logger.logDebug("Stop timer");
    this->stopCountdown();
}

void TimerPort::startCountdown(Duration duration)
{
    auto countdown_func = [this](Duration duration)
    {
        std::cv_status wait_result {};
        std::unique_lock<std::mutex> mutex_uniq_cv(this->countdown_guard);
        wait_result = this->countdown_cv.wait_for(mutex_uniq_cv, duration);

        switch(wait_result)
        {
            case std::cv_status::timeout:;
            {
                this->handler->handleTimeout();
                return;
            }
            break;
            case std::cv_status::no_timeout:;
            {
                return;
            }
            break;
            default:
                MARK_UNREACHABLE();
            break;
        }

        MARK_UNREACHABLE();
    };

    // This starts countdown
    std::thread countdown_thread(countdown_func, duration);
    countdown_thread.detach();
}

void TimerPort::stopCountdown()
{
    this->countdown_cv.notify_all();
}

}
