#pragma once

#include <SDL.h>

#include <array>
#include <optional>

#include "assets/assets.h"

static constexpr uint8_t RENDER_LAYERS = 5;

struct CRender {
    uint8_t layer = 0;
};

struct CSprite {
    assets::AssetId textureId;
    std::optional<unsigned int> width;
    std::optional<unsigned int> height;
};

struct CText {
    assets::AssetId fontId;
    SDL_Color color;
    size_t text;
};

