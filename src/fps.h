#pragma once

#include <SDL.h>

#include <chrono>  // NOLINT [build/c++11]
#include <optional>

#include "./logging.h"

class FPSCounter {
 public:
    FPSCounter() : m_lastReportTime(std::nullopt) {}
    void report() {
        auto now = std::chrono::system_clock::now();
        if (!m_lastReportTime.has_value()) {
            LOG_INFO("Starting FPS Counter");
            m_lastReportTime = std::make_optional(now);
        } else {
            double dt = std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(now - m_lastReportTime.value()).count();
            LOG_INFO("FPS: %f", m_numFrames / dt);
            m_numFrames = 0;
            m_lastReportTime = std::make_optional(now);
        }
    }
    void incrementFrames() {
        m_numFrames++;
    }
    void stop() {
        m_lastReportTime = std::nullopt;
    }

 private:
    size_t m_numFrames;
    std::optional<std::chrono::time_point<std::chrono::system_clock>> m_lastReportTime;
};

FPSCounter g_fpsCounter;

