
#include <memory>

#include "../ecs.h"
#include "../assets.h"
#include "../renderer.h"
#include "../texture.h"

class RenderSystem : public System {
    public:
        void update(double dt) override {
            for (Entity entity : m_entities) {
                CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
                std::shared_ptr<Texture> t = g_textures.get(sprite.textureId);
                SDL_Rect rect;
                CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
                rect.x = (int) translation.x;
                rect.y = (int) translation.y;
                rect.w = sprite.width.value_or(t->m_w);
                rect.h = sprite.height.value_or(t->m_h);
                SDL_RenderCopy(g_renderer, t->m_img, nullptr, &rect);
            }
        }
};

