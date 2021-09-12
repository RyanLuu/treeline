#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <queue>

using Entity = std::uint16_t;
const size_t MAX_ENTITIES = 4096;
const size_t MAX_COMPONENTS = 32;
using Archetype = std::bitset<MAX_COMPONENTS>;

class EntityManager {
 public:
    EntityManager() {
        for (Entity e = 0; e < MAX_ENTITIES; e++) {
            m_freeList.push(e);
        }
    }

    Entity createEntity() {
        assert(m_numEntities < MAX_ENTITIES);
        Entity e = m_freeList.front();
        m_freeList.pop();
        m_numEntities++;
        m_archetypes[e].reset();
        return e;
    }

    void destroyEntity(Entity e) {
        assert(e < MAX_ENTITIES);
        m_freeList.push(e);
        m_numEntities--;
    }

    Archetype getArchetype(Entity e) {
        assert(e < MAX_ENTITIES);
        return m_archetypes[e];
    }

    void setArchetype(Entity e, Archetype archetype) {
        assert(e < MAX_ENTITIES);
        m_archetypes[e] = archetype;
    }

 private:
    std::queue<Entity> m_freeList;
    std::array<Archetype, MAX_ENTITIES> m_archetypes;
    size_t m_numEntities;
};

