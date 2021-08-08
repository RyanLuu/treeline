#include "../ecs.h"

class VelocitySystem : public System {
    public:
        void update(unsigned int dt) override {
            for (Entity entity : m_entities) {
                CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
                CVelocity &velocity = g_ecs.getComponent<CVelocity>(entity);
                translation.x += velocity.vx * dt / 1000;
                translation.y += velocity.vy * dt / 1000;
            }
        }
};

