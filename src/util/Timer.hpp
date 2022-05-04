#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace the {
    class SimpleTimer {

    public:
        SimpleTimer(float interval) noexcept;
        SimpleTimer(const SimpleTimer&) = delete;
        SimpleTimer(SimpleTimer&&) = delete;

        SimpleTimer& operator=(const SimpleTimer&) = delete;
        SimpleTimer& operator=(SimpleTimer&&) = delete;

        void setNewInterval(float interval) noexcept;
        void markPoint() noexcept;
        bool intervalElapsed() const noexcept;
        void nextInterval() noexcept;

        std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> getElapsedTime() const noexcept;

    private:
        std::chrono::time_point<std::chrono::steady_clock> currTime_;
        std::chrono::time_point<std::chrono::steady_clock> prevTime_;
        float accumulator_;
        float intervalSeconds_;
    };
}

#endif // TIMER_H