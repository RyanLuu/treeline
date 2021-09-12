#pragma once

#include <memory>
#include <unordered_set>

#include "core/component.h"
#include "core/entity.h"
#include "core/system.h"
#include "logging.h"

class ECS {
 public:
    ECS() {
        m_entityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_updateSystemManager = std::make_unique<SystemManager>();
        m_renderSystemManager = std::make_unique<SystemManager>();
    }

    template <typename... Ts>
    Archetype createArchetype(Ts... components) {
        Component ids[] = {components...};
        Archetype ret;
        for (Component id : ids) {
            ret.set(id);
        }
        return ret;
    }

    Entity createEntity() {
        return m_entityManager->createEntity();
    }

    void destroyEntity(Entity e) {
        // schedule to be destroyed after system updates so that
        // (a) iterating over all entities is easier
        // (b) there is less dependence on system order
        m_destroyList.insert(e);
    }
    template <typename T>
    void registerComponent() {
        m_componentManager->registerComponent<T>();
    }
    template <typename T>
    void addComponents(Entity e, T component) {
        m_componentManager->addComponent<T>(e, component);

        // update entity's archetype
        Archetype archetype = m_entityManager->getArchetype(e);
        archetype.set(m_componentManager->getId<T>());
        m_entityManager->setArchetype(e, archetype);

        // notify system manager of change
        m_updateSystemManager->notifyChanged(e, archetype);
        m_renderSystemManager->notifyChanged(e, archetype);
    }

    template <typename T, typename... Ts>
    void addComponents(Entity e, T component, Ts... components) {
        m_componentManager->addComponent<T>(e, component);

        // update entity's archetype
        Archetype archetype = m_entityManager->getArchetype(e);
        archetype.set(m_componentManager->getId<T>());
        m_entityManager->setArchetype(e, archetype);

        // recursive call
        addComponents(e, components...);
    }

    template <typename T>
    void removeComponent(Entity e) {
        m_componentManager->removeComponent<T>(e);

        // update entity's archetype
        Archetype archetype = m_entityManager->getArchetype(e);
        archetype.reset(m_componentManager->getId<T>());
        m_entityManager->setArchetype(e, archetype);

        // notify system manager of change
        m_updateSystemManager->notifyChanged(e, archetype);
        m_renderSystemManager->notifyChanged(e, archetype);
    }

    template <typename T>
    T &getComponent(Entity e) {
        return m_componentManager->getComponent<T>(e);
    }

    template <typename T>
    Component getComponentId() {
        return m_componentManager->getId<T>();
    }

    template <typename T>
    std::shared_ptr<T> registerSystem(Archetype archetype = Archetype{}) {
        return m_updateSystemManager->registerSystem<T>(archetype);
    }

    template <typename T>
    std::shared_ptr<T> registerRenderSystem(Archetype archetype = Archetype{}) {
        return m_renderSystemManager->registerSystem<T>(archetype);
    }

    template <typename T>
    void setSystemArchetype(Archetype archetype) {
        if (m_updateSystemManager->hasSystem<T>()) {
            m_updateSystemManager->setArchetype<T>(archetype);
        }
        if (m_renderSystemManager->hasSystem<T>()) {
            m_renderSystemManager->setArchetype<T>(archetype);
        }
    }

    void updateSystems(uint64_t dt) {
        m_updateSystemManager->update(dt);
        m_renderSystemManager->update(dt);

        for (Entity e : m_destroyList) {
            // deallocate entity ID for future use
            m_entityManager->destroyEntity(e);
            // delete component data
            m_componentManager->notifyDestroyed(e);
            // remove entity from all systems
            m_updateSystemManager->notifyDestroyed(e);
            m_renderSystemManager->notifyDestroyed(e);
        }
        m_destroyList.clear();
    }

 private:
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<SystemManager> m_updateSystemManager;
    std::unique_ptr<SystemManager> m_renderSystemManager;
    std::unordered_set<Entity> m_destroyList;
};

extern ECS g_ecs;

