#pragma once

#include <chrono>
#include <SDL2/SDL.h>

class FPSCounter {
    public:
        FPSCounter() : m_lastReportTime(std::nullopt) {}
        void report() {
            auto now = std::chrono::system_clock::now();
            if (!m_lastReportTime.has_value()) {
                SDL_Log("Starting FPS Counter");
                m_lastReportTime = std::make_optional(now);
            } else {
                double dt = std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(now - m_lastReportTime.value()).count();
                SDL_Log("FPS: %f", m_numFrames / dt);
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

