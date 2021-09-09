#pragma once

#include <memory>

#include "../assets.h"
#include "../components/render.h"
#include "../components/transform.h"
#include "../ecs/ecs.h"
#include "../renderer.h"
#include "../texture.h"

class RenderSystem : public System {
 public:
    void update(double) override {
        for (Entity entity : m_entities) {
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
};

