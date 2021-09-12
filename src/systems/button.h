#pragma once

#include "components/button.h"
#include "components/render.h"
#include "core/ecs.h"
#include "io/keyboard.h"

class ButtonSystem : public System {
 public:
    void update(uint64_t) override {
        for (Entity entity : m_entities) {
            CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
            CButton &button = g_ecs.getComponent<CButton>(entity);
            sprite.textureId = g_keyboard.get(button.key) ? button.activeSprite : button.idleSprite;
        }
    }
};

