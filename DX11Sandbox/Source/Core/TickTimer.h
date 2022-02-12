#pragma once
#include <chrono>

class TickTimer
{
public:
    TickTimer();

    void Update();

    inline double GetAccumulatedTime()
    {
        return time_accumulator_;
    }

    static inline constexpr double TICK_TIME = 1.0 / 60.0;  // in seconds
    static inline constexpr uint32_t MAX_TICKS_PER_FRAME = 60;

private:
    std::chrono::high_resolution_clock::time_point current_;
    std::chrono::high_resolution_clock::time_point prev_;

    double time_accumulator_ = 0;
};
