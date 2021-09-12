#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#include "assets/assets.h"
#include "core/logging.h"
#include "io/renderer.h"

static constexpr char FontRoot[] = "assets/fonts/";

class Font : public Asset {
   public:
    void load(const std::string &filepath) override {
        m_ttf = TTF_OpenFont((FontRoot + filepath).c_str(), 24);
        assert(m_ttf != nullptr);
        LOG_INFO("Loaded font [%s]", filepath.c_str());
    }

    void unload() override {
        TTF_CloseFont(m_ttf);
    }

    TTF_Font *m_ttf;
};

extern Assets<Font> g_fonts;

