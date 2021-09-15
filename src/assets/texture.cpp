#include "assets/texture.h"

namespace assets {

void Texture::load(const std::string &filepath) {
    m_img = IMG_LoadTexture(g_renderer, (ImageRoot + filepath).c_str());
    assert(m_img != nullptr);
    SDL_QueryTexture(m_img, nullptr, nullptr, &m_w, &m_h);
    LOG_INFO("Loaded texture [%s]", filepath.c_str());
}

void Texture::unload() {
    SDL_DestroyTexture(m_img);
}

Assets<Texture> g_textures;

}  // namespace assets

