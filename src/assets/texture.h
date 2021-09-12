#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <string>

#include "assets/assets.h"
#include "core/logging.h"
#include "io/renderer.h"

static constexpr char ImageRoot[] = "assets/images/";

class Texture : public Asset {
   public:
    void load(const std::string &filepath) override {
        m_img = IMG_LoadTexture(g_renderer, (ImageRoot + filepath).c_str());
        assert(m_img != nullptr);
        SDL_QueryTexture(m_img, nullptr, nullptr, &m_w, &m_h);
        LOG_INFO("Loaded texture [%s]", filepath.c_str());
    }

    void unload() override {
        SDL_DestroyTexture(m_img);
    }

    SDL_Texture *m_img;
    int m_w;
    int m_h;
};

extern Assets<Texture> g_textures;

