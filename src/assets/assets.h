#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace assets {

using AssetId = uint32_t;

/**
 *  Unit of data that can be loaded from disk into an in-memory representation.
 */
class Asset {
   public:
    virtual ~Asset() = default;
    virtual void load(const std::string &filepath) = 0;
    virtual void unload() = 0;
};

/**
 *  
 */
template <typename T>
class Assets {
   public:
    Assets();
    AssetId load(const std::string &filepath);
    std::shared_ptr<T> get(AssetId id);
    void unload(AssetId id);
    void unloadAll();

   private:
    std::unordered_map<AssetId, std::shared_ptr<T>> m_resources;
    AssetId m_currentId;
};

#include "assets/assets.ipp"

}  // namespace assets

