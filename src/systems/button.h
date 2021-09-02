#include "../ecs.h"
#include "../keyboard.h"

class ButtonSystem : public System {
    public:
        void update(double dt) override {
            for (Entity entity : m_entities) {
                CSprite &sprite = g_ecs.getComponent<CSprite>(entity);
                CButton &button = g_ecs.getComponent<CButton>(entity);
                sprite.textureId = g_keyboard.get(button.keycode) ? button.activeSprite : button.idleSprite;
            }
        }
};

