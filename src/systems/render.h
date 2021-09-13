#pragma once

#include <array>
#include <memory>
#include <unordered_set>

#include "assets/assets.h"
#include "assets/font.h"
#include "assets/texture.h"
#include "components/render.h"
#include "components/transform.h"
#include "core/ecs.h"
#include "io/renderer.h"

static constexpr std::array<const char *, 3> TEXT_POOL =
    {{"Hello world",
      "Goodbye",
      "string 3"}};

class RenderSystem : public System {
   public:
    void update(uint64_t) override {
        for (Entity entity : m_entities) {
            CRender &renderData = g_ecs.getComponent<CRender>(entity);
            m_layers[renderData.layer].emplace(entity);
        }
        for (auto layer : m_layers) {
            for (Entity entity : layer) {
                CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
                if (g_ecs.hasComponent<CSprite>(entity)) {
                    CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
                    std::shared_ptr<assets::Texture> t = assets::g_textures.get(sprite.textureId);
                    SDL_Rect rect;
                    rect.x = static_cast<int>(translation.x);
                    rect.y = static_cast<int>(translation.y);
                    rect.w = sprite.width.value_or(t->m_w);
                    rect.h = sprite.height.value_or(t->m_h);
                    SDL_RenderCopy(g_renderer, t->m_img, nullptr, &rect);
                }
                if (g_ecs.hasComponent<CText>(entity)) {
                    CText &text = g_ecs.getComponent<CText>(entity);
                    std::shared_ptr<assets::Font> font = assets::g_fonts.get(text.fontId);
                    SDL_Surface *surface = TTF_RenderText_Blended(font->m_ttf, TEXT_POOL[text.text], text.color);
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderer, surface);
                    SDL_Rect rect;
                    rect.x = static_cast<int>(translation.x);
                    rect.y = static_cast<int>(translation.y);
                    SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
                    SDL_RenderCopy(g_renderer, texture, nullptr, &rect);
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                }
            }
        }
        for (auto &layer : m_layers) {
            layer.clear();
        }
    }

   private:
    std::array<std::unordered_set<Entity>, RENDER_LAYERS> m_layers;
};

