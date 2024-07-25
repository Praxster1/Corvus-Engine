//
// Created by timow on 25/07/2024.
//

#ifndef ENGINE_TIMER_H
#define ENGINE_TIMER_H

#include <map>

namespace Corvus
{

    class Timer {
    public:

        static void init() {
            spdlog::set_level(spdlog::level::trace);
            CORVUS_LOG(info, "Timer activated.");
        }

        static void destroy() {
            spdlog::set_level(spdlog::level::info);
            getInstance().timers.clear();
            CORVUS_LOG(info, "Timer deactivated.");
        }

        static Timer& getInstance() {
            static Timer instance;
            return instance;
        }

        void start(const std::string& name) {
            auto startTime = std::chrono::high_resolution_clock::now();
            timers[name] = startTime;
        }

        void stop(const std::string& name) {
            auto endTime = std::chrono::high_resolution_clock::now();
            if (timers.find(name) != timers.end()) {
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - timers[name]).count();
                CORVUS_LOG(trace, "Timer [{}] took {} microseconds.", name, duration);
                timers.erase(name);  // Remove the timer after stopping
            } else {
                CORVUS_LOG(error, "Timer [{}] was not started.", name);
            }
        }

    private:
        Timer() = default;
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;
    };
} // Corvus

#endif //ENGINE_TIMER_H
