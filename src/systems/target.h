#pragma once

#include <SDL.h>

#include "assets/assets.h"
#include "assets/texture.h"
#include "components/render.h"
#include "components/tags.h"
#include "components/transform.h"
#include "core/ecs.h"

class TargetSystem : public System {
   public:
    void update(uint64_t) override {
        /*
        if (rand() % 100 == 0) {
            Entity newEntity = g_ecs.createEntity();
            g_ecs.addComponents<CSprite, CTranslation, CVelocity, CTargetTag>(newEntity, CSprite{m_sprite, 50, 50}, CTranslation{100.f * (rand() % 4 + 1), 500}, CVelocity{0, -300}, CTargetTag{});
        }
	*/
        for (Entity e : m_entities) {
            CTranslation &translation = g_ecs.getComponent<CTranslation>(e);
            if (translation.y < 0) {
                g_ecs.destroyEntity(e);
            }
        }
    }
};

