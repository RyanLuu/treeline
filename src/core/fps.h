#pragma once

#include <SDL.h>

#include <optional>

#include "core/logging.h"
#include "core/timer.h"

class FPSCounter {
 public:
    FPSCounter() : m_lastReportTime(std::nullopt) {}
    void report() {
        uint64_t now = time_now();
        if (!m_lastReportTime.has_value()) {
            LOG_INFO("Starting FPS Counter");
            m_lastReportTime = std::make_optional(now);
        } else {
            LOG_INFO("FPS: %f", static_cast<float>(m_numFrames) / (now - m_lastReportTime.value()) * 1'000'000);
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
    std::optional<uint64_t> m_lastReportTime;
};

FPSCounter g_fpsCounter;

