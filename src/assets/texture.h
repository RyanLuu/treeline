#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <string>

#include "assets/assets.h"
#include "core/logging.h"
#include "io/renderer.h"

namespace assets {

static constexpr char ImageRoot[] = "assets/images/";

class Texture : public Asset {
   public:
    void load(const std::string &filepath) override;
    void unload() override;

    SDL_Texture *m_img;
    int m_w;
    int m_h;
};

extern Assets<Texture> g_textures;

}  // namespace assets

