#pragma once

#include <chrono>  // NOLINT [build/c++11]
#include <functional>
#include <thread>  // NOLINT [build/c++11]

uint64_t time_now() {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    return micros.count();
}

class Timer {
 public:
    explicit Timer(std::function<void(void)> func, uint32_t interval = 0) : m_func(func), m_interval(interval), m_running(false) {}

    void start() {
        assert(!m_running);
        m_running = true;
        if (m_interval > 0) {
            m_thread = std::thread([this]() {
                while (m_running) {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    auto next = std::chrono::steady_clock::now() + std::chrono::microseconds(m_interval);
                    m_func();
                    m_cv.wait_until(lock, next);
                }
            });
        } else {
            m_thread = std::thread([this]() {
                while (m_running) {
                    m_func();
                    std::this_thread::yield();
                }
            });
        }
    }

    void stop() {
        assert(m_running);
        m_running = false;
        m_cv.notify_all();
        m_thread.join();
    }

    void toggle() {
        if (isRunning()) {
            stop();
        } else {
            start();
        }
    }

    bool isRunning() { return m_running; }

 private:
    std::function<void(void)> m_func;
    unsigned int m_interval;
    bool m_running;
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
};

