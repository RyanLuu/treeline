#pragma once

#include <array>
#include <memory>
#include <unordered_set>

#include "assets/assets.h"
#include "components/render.h"
#include "components/transform.h"
#include "core/ecs.h"
#include "io/renderer.h"
#include "assets/texture.h"

class RenderSystem : public System {
 public:
    void update(uint64_t) override {
        for (Entity entity : m_entities) {
            CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
            m_layers[sprite.layer].emplace(entity);
        }
        for (auto layer : m_layers) {
            for (Entity entity : layer) {
                CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
                std::shared_ptr<Texture> t = g_textures.get(sprite.textureId);
                SDL_Rect rect;
                CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
                rect.x = static_cast<int>(translation.x);
                rect.y = static_cast<int>(translation.y);
                rect.w = sprite.width.value_or(t->m_w);
                rect.h = sprite.height.value_or(t->m_h);
                SDL_RenderCopy(g_renderer, t->m_img, nullptr, &rect);
            }
        }
        for (auto &layer : m_layers) {
            layer.clear();
        }
    }

 private:
    std::array<std::unordered_set<Entity>, RENDER_LAYERS> m_layers;
};

