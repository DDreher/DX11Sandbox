#include "TickTimer.h"

TickTimer::TickTimer()
    : current_(std::chrono::high_resolution_clock::now())
    , prev_(std::chrono::high_resolution_clock::now())
{
}

void TickTimer::Update()
{
    current_ = std::chrono::high_resolution_clock::now();
    const double elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(current_ - prev_).count());
    prev_ = current_;
    time_accumulator_ += elapsed;

    // Cap so we don't get weird artifacts when debugging
    time_accumulator_ = std::min(time_accumulator_, TICK_TIME * MAX_TICKS_PER_FRAME);
}
