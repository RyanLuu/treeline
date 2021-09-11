#pragma once

#include <optional>

#include "src/assets.h"

struct CSprite {
    AssetId textureId;
    std::optional<unsigned int> width;
    std::optional<unsigned int> height;
};

