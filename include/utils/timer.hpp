#ifndef QUERYLENS_TIMER_HPP
#define QUERYLENS_TIMER_HPP

#include <chrono>
#include <string>

namespace querylens {

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool running_;

public:
    Timer() : running_(false) {}

    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
        running_ = true;
    }

    void stop() {
        end_time_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }

    // Returns elapsed time in microseconds
    double elapsed_us() const {
        auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_).count();
    }

    // Returns elapsed time in milliseconds
    double elapsed_ms() const {
        return elapsed_us() / 1000.0;
    }

    // Returns elapsed time in seconds
    double elapsed_s() const {
        return elapsed_us() / 1000000.0;
    }
};

} // namespace querylens

#endif // QUERYLENS_TIMER_HPP
