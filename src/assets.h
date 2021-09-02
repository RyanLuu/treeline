#pragma once

#include <unordered_map>
#include <memory>
#include <string>

class Asset {
    public:
        virtual ~Asset() = default;
        virtual void load(const std::string &filepath) = 0;
        virtual void unload() = 0;
};

template<typename T>
class Assets {
    public:
        unsigned int load(const std::string &filepath);
        std::shared_ptr<T> get(unsigned int id);
        void unload(unsigned int id);
        void unloadAll();
    private:
        std::unordered_map<unsigned int, std::shared_ptr<T>> m_resources;
        unsigned int m_currentId;
};

#include "assets.ipp"

