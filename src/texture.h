#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "assets.h"

class Texture : public Asset {
    public:
        void load(const std::string &filepath) override;
        void unload() override;
        SDL_Texture *m_img;
        int m_w;
        int m_h;
};

extern Assets<Texture> g_textures;

