#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "renderer.h"

static constexpr char ImageRoot[] = "assets/images/";

void Texture::load(const std::string &filepath) {
    m_img = IMG_LoadTexture(g_renderer, (ImageRoot + filepath).c_str());
    assert(m_img != nullptr);
    SDL_QueryTexture(m_img, nullptr, nullptr, &m_w, &m_h);
    SDL_Log("Loaded texture [%s]", filepath.c_str());
}

void Texture::unload() {
    SDL_DestroyTexture(m_img);
}

