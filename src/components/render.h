#pragma once

#include <optional>

#include "assets/assets.h"

static constexpr uint8_t RENDER_LAYERS = 4;

struct CSprite {
    AssetId textureId;
    std::optional<unsigned int> width;
    std::optional<unsigned int> height;
    uint8_t layer = 0;
};

