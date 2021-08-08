#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>

using Entity = std::uint32_t;
const size_t MAX_ENTITIES = 4096;

using Component = std::uint8_t;
const size_t MAX_COMPONENTS = 32;

using Archetype = std::bitset<MAX_COMPONENTS>;

class EntityManager {
    public:
        EntityManager();
        Entity createEntity();
        void destroyEntity(Entity e);
        Archetype getArchetype(Entity e);
        void setArchetype(Entity e, Archetype archetype);
    private:
        std::queue<Entity> m_freeList;
        std::array<Archetype, MAX_ENTITIES> m_archetypes;
        size_t m_numEntities;
};

class IComponentData {
    public:
        virtual ~IComponentData() = default;
        virtual void notifyDestroyed(Entity e) = 0;
};

template<typename T>
class ComponentData : public IComponentData {
    public:
        void insert(Entity e, T component);
        void remove(Entity e);
        T &get(Entity e);
        void notifyDestroyed(Entity e) override;
    private:
        std::array<T, MAX_ENTITIES> m_data;
        std::unordered_map<Entity, size_t> m_entityToIndex;
        std::unordered_map<size_t, Entity> m_indexToEntity;
        size_t m_size;
};

class ComponentManager {
    public:
        template<typename T> void registerComponent();
        template<typename T> void addComponent(Entity e, T component);
        template<typename T> void removeComponent(Entity e);
        template<typename T> T &getComponent(Entity e);
        template<typename T> Component getId();
        void notifyDestroyed(Entity e);
    private:
        template<typename T>
            std::shared_ptr<ComponentData<T>> getComponentData() {
                std::size_t typeName = typeid(T).hash_code();
                assert(m_idMap.find(typeName) != m_idMap.end());
                return std::static_pointer_cast<ComponentData<T>>(m_dataMap[typeName]);
            }
        std::unordered_map<std::size_t, std::shared_ptr<IComponentData>> m_dataMap;
        std::unordered_map<std::size_t, Component> m_idMap;
        size_t m_numComponents;
};

class System {
    public:
        virtual ~System() = default;
        std::set<Entity> m_entities;
        virtual void update(unsigned int dt) = 0;
};

class SystemManager {
    public:
        template<typename T> std::shared_ptr<T> registerSystem(Archetype archetype);
        template<typename T> void setArchetype(Archetype archetype);
        void notifyDestroyed(Entity e);
        void notifyChanged(Entity e, Archetype archetype);
        void update(unsigned int dt);
    private:
        std::unordered_map<std::shared_ptr<System>, Archetype> m_systems;
};

class ECS {
    public:
        ECS();
        template<typename... Ts> Archetype createArchetype(Ts... components);
        Entity createEntity();
        void destroyEntity(Entity e);
        template<typename T> void registerComponent();
        template<typename T> void addComponents(Entity e, T component);
        template<typename T, typename... Ts> void addComponents(Entity e, T component, Ts... components);
        template<typename T> void removeComponent(Entity e);
        template<typename T> T &getComponent(Entity e);
        template<typename T> Component getComponentId();
        template<typename T> std::shared_ptr<T> registerSystem(Archetype archetype = Archetype{});
        template<typename T> void setSystemArchetype(Archetype archetype);
        void updateSystems(unsigned int dt);
    private:
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<SystemManager> m_systemManager;
};

extern ECS g_ecs;

#include "ecs.tpp"

