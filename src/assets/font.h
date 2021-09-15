#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#include "assets/assets.h"
#include "core/logging.h"
#include "io/renderer.h"

namespace assets {

static constexpr char FontRoot[] = "assets/fonts/";

class Font : public Asset {
   public:
    void load(const std::string &filepath) override;
    void unload() override;
    TTF_Font *m_ttf;
};

extern Assets<Font> g_fonts;

}  // namespace assets

