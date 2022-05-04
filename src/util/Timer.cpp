#include "Timer.hpp"

the::SimpleTimer::SimpleTimer(float seconds) noexcept :
	intervalSeconds_{ seconds },
	currTime_{},
	prevTime_{},
	accumulator_{seconds}
{}

void the::SimpleTimer::markPoint() noexcept {
	currTime_ = std::chrono::steady_clock::now();
	std::chrono::duration<float> timeElapsed = currTime_ - prevTime_;
	prevTime_ = currTime_;
	accumulator_ += timeElapsed.count();
	return;
}

bool the::SimpleTimer::intervalElapsed() const noexcept {
	return accumulator_ >= intervalSeconds_;
 }

void the::SimpleTimer::setNewInterval(float interval) noexcept {
	intervalSeconds_ = interval;
	this->nextInterval();
}

void the::SimpleTimer::nextInterval() noexcept {
	accumulator_ = 0.f;
}

std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> the::SimpleTimer::getElapsedTime() const noexcept
{
	return std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>();
}
