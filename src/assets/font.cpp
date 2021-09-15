#include "assets/font.h"

namespace assets {

void Font::load(const std::string &filepath) {
    m_ttf = TTF_OpenFont((FontRoot + filepath).c_str(), 24);
    assert(m_ttf != nullptr);
    LOG_INFO("Loaded font [%s]", filepath.c_str());
}

void Font::unload() {
    TTF_CloseFont(m_ttf);
}

Assets<Font> g_fonts;

}  // namespace assets

