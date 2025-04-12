#pragma once

#include <chrono>
#include <string>
#include <functional>

#include "log.h"

namespace hbqj {
    class ScopedTimer {
    public:
        ScopedTimer(std::string name) : name_(std::move(name)), start_(std::chrono::high_resolution_clock::now()) {}

        ~ScopedTimer() {
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);

            auto seconds = duration.count() / 1000;
            auto milliseconds = duration.count() % 1000;

            if (seconds > 0) {
                log.info("{} took {}s {:03d}ms.", name_, seconds, milliseconds);
            } else {
                log.info("{} took {:03d}ms.", name_, milliseconds);
            }
        }

    private:
        std::string name_;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_;
        Logger log = Logger::GetLogger("ScopedTimer");
    };

#define MEASURE_TIME(name) ScopedTimer timer##__LINE__(name)
}