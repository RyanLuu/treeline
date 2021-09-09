#pragma once

#include "src/components/button.h"
#include "src/components/render.h"
#include "src/ecs/ecs.h"
#include "src/keyboard.h"

class ButtonSystem : public System {
 public:
    void update(double) override {
        for (Entity entity : m_entities) {
            CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
            CButton &button = g_ecs.getComponent<CButton>(entity);
            sprite.textureId = g_keyboard.get(button.key) ? button.activeSprite : button.idleSprite;
        }
    }
};

