#pragma once

#include <chrono>  // NOLINT [build/c++11]
#include <functional>
#include <thread>  // NOLINT [build/c++11]

class Timer {
 public:
    explicit Timer(std::function<void(void)> func, unsigned int interval = 0) : m_running(false) {
        if (interval > 0) {
            m_thread = std::thread([this, func, interval]() {
                while (m_running) {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
                    func();
                    m_cv.wait_until(lock, next);
                }
            });
        } else {
            m_thread = std::thread([this, func]() {
                while (m_running) {
                    func();
                }
            });
        }
    }
    void start() {
        m_running = true;
    }
    void stop() {
        m_running = false;
        m_cv.notify_all();
        m_thread.join();
    }

 private:
    bool m_running;
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
};

