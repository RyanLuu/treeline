#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Asset {
 public:
    virtual ~Asset() = default;
    virtual void load(const std::string &filepath) = 0;
    virtual void unload() = 0;
};

template <typename T>
class Assets {
 public:
    unsigned int load(const std::string &filepath) {
        std::shared_ptr<T> resource = std::make_shared<T>();
        resource->load(filepath);
        m_resources.insert({m_currentId, resource});
        return m_currentId++;
    }
    std::shared_ptr<T> get(unsigned int id) {
        auto it = m_resources.find(id);
        if (it == m_resources.end()) {
            return nullptr;
        }
        return it->second;
    }
    void unload(unsigned int id) {
        for (auto it = m_resources.begin(); it != m_resources.end(); it++) {
            if (it->first == id) {
                it->second->unload();
                m_resources.erase(it->first);
                return;
            }
        }
    }
    void unloadAll() {
        for (auto it = m_resources.begin(); it != m_resources.end(); it++) {
            it->second->unload();
        }
        m_resources.clear();
    }

 private:
    std::unordered_map<unsigned int, std::shared_ptr<T>> m_resources;
    unsigned int m_currentId;
};

