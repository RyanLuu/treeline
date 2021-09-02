#include "../ecs.h"

class VelocitySystem : public System {
    public:
        void update(double dt) override {
            for (Entity entity : m_entities) {
                CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
                CVelocity &velocity = g_ecs.getComponent<CVelocity>(entity);
                translation.x += velocity.vx * dt;
                translation.y += velocity.vy * dt;
            }
        }
};

