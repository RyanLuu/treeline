#pragma once

#include "src/components/transform.h"
#include "src/ecs/ecs.h"

class VelocitySystem : public System {
 public:
    void update(uint64_t dt) override {
        for (Entity entity : m_entities) {
            CTranslation &translation = g_ecs.getComponent<CTranslation>(entity);
            CVelocity &velocity = g_ecs.getComponent<CVelocity>(entity);
            translation.x += velocity.vx * dt / 1'000'000;
            translation.y += velocity.vy * dt / 1'000'000;
        }
    }
};

