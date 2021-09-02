#pragma once

#include <any>
#include <array>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

enum class EventType : size_t {
    QUIT, KEYBOARD,
    Count  // hack to get the number of event types
};

struct Event {
    EventType type;
    std::unordered_map<std::string, std::any> params;

    template<typename T>
    const T getParam(std::string key) const {
        auto param = params.find(key);
        assert(param != params.end());
        return std::any_cast<T>(param->second);
    }
};

class EventManager {
    public:
        void addListener(EventType type, const std::function<void(const Event &)> &listener) {
            m_listeners[static_cast<size_t>(type)].emplace_back(listener);
        }

        void sendEvent(Event event) {
            for (auto listener : m_listeners[static_cast<size_t>(event.type)]) {
                listener(event);
            }
        }

    private:
        std::array<std::vector<std::function<void(const Event &)>>, static_cast<size_t>(EventType::Count)> m_listeners;
};

extern EventManager g_eventManager;

