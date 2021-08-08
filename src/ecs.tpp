EntityManager::EntityManager() {
    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        m_freeList.push(e);
    }
}

Entity EntityManager::createEntity() {
    assert(m_numEntities < MAX_ENTITIES);
    Entity e = m_freeList.front();
    m_freeList.pop();
    m_numEntities++;
    return e;
}

void EntityManager::destroyEntity(Entity e) {
    assert(e < MAX_ENTITIES);
    m_archetypes[e].reset();
    m_freeList.push(e);
    m_numEntities++;
}

Archetype EntityManager::getArchetype(Entity e) {
    assert(e < MAX_ENTITIES);
    return m_archetypes[e];
}

void EntityManager::setArchetype(Entity e, Archetype archetype) {
    assert(e < MAX_ENTITIES);
    m_archetypes[e] = archetype;
}

template <typename T>
void ComponentData<T>::insert(Entity e, T component) {
    assert(m_entityToIndex.find(e) == m_entityToIndex.end());
    size_t index = m_size;
    m_entityToIndex[e] = index;
    m_indexToEntity[index] = e;
    m_data[index] = component;
    m_size++;
}

template <typename T>
void ComponentData<T>::remove(Entity e) {
    assert(m_entityToIndex.find(e) != m_entityToIndex.end());
    size_t indexRemove = m_entityToIndex[e];
    size_t indexLast = m_size - 1;
    size_t entityLast = m_indexToEntity[indexLast];

    m_data[indexRemove] = m_data[indexLast];
    m_indexToEntity[indexRemove] = entityLast;
    m_entityToIndex[e] = indexLast;

    m_entityToIndex.erase(e);
    m_indexToEntity.erase(indexRemove);

    m_size--;
}

template <typename T>
T &ComponentData<T>::get(Entity e) {
    assert(m_entityToIndex.find(e) != m_entityToIndex.end());
    return m_data[m_entityToIndex[e]];
}

template <typename T>
void ComponentData<T>::notifyDestroyed(Entity e) {
    if (m_entityToIndex.find(e) != m_entityToIndex.end()) {
        remove(e);
    }
}

template<typename T>
void ComponentManager::registerComponent() {
    assert(m_numComponents < MAX_COMPONENTS);
    std::size_t typeHash = typeid(T).hash_code();
    assert(m_idMap.find(typeHash) == m_idMap.end());
    m_idMap.insert({typeHash, m_numComponents});
    m_dataMap.insert({typeHash, std::make_shared<ComponentData<T>>()});
    m_numComponents++;
}

template<typename T>
void ComponentManager::addComponent(Entity e, T component) {
    getComponentData<T>()->insert(e, component);
}

template<typename T>
void ComponentManager::removeComponent(Entity e) {
    getComponentData<T>()->remove(e);
}

template<typename T>
T &ComponentManager::getComponent(Entity e) {
    return getComponentData<T>()->get(e);
}

template<typename T>
Component ComponentManager::getId() {
    std::size_t typeHash = typeid(T).hash_code();
    assert(m_idMap.find(typeHash) != m_idMap.end());
    return m_idMap[typeHash];
}


void ComponentManager::notifyDestroyed(Entity e) {
    for (const auto &pair : m_dataMap) {
        pair.second->notifyDestroyed(e);
    }
}


template<typename T>
std::shared_ptr<T> SystemManager::registerSystem(Archetype archetype) {
    static_assert(std::is_base_of<System, T>::value, "T must inherit from System");

    // ensure system of the same type does not already exist
    std::size_t typeHash = typeid(T).hash_code();
    assert(std::none_of(m_systems.begin(), m_systems.end(), [typeHash](const auto &pair) {
        return typeHash == typeid(*pair.first).hash_code();
    }));

    // create pointer to new system of type T
    std::shared_ptr<T> system = std::make_shared<T>();
    m_systems.insert({system, archetype});
    return system;
}

template<typename T>
void SystemManager::setArchetype(Archetype archetype) {
    static_assert(std::is_base_of<System, T>::value, "T must inherit from System");

    // ensure system of the same type does not already exist
    std::size_t typeHash = typeid(T).hash_code();
    const auto &pair = std::find_if(m_systems.begin(), m_systems.end(), [typeHash](const auto &pair) {
        return typeHash == typeid(*pair.first).hash_code();
    });
    assert(pair != m_systems.end());
    pair.second = archetype;
}

void SystemManager::notifyDestroyed(Entity e) {
    for (const auto &pair : m_systems) {
        pair.first->m_entities.erase(e);
    }
}

void SystemManager::notifyChanged(Entity e, Archetype archetype) {
    for (const auto &pair : m_systems) {
        std::shared_ptr<System> system = pair.first;
        Archetype systemArchetype = pair.second;
        if ((archetype & systemArchetype) == systemArchetype) {
            // add to system if signature matches
            system->m_entities.insert(e);
        } else {
            // remove from system if signature doesn't match
            system->m_entities.erase(e);
        }
    }
}

void SystemManager::update(unsigned int dt) {
    for (const auto &pair : m_systems) {
        pair.first->update(dt);
    }
}

ECS::ECS() {
	m_entityManager = std::make_unique<EntityManager>();
    m_componentManager = std::make_unique<ComponentManager>();
	m_systemManager = std::make_unique<SystemManager>();
}

template <typename... Ts>
Archetype ECS::createArchetype(Ts... components) {
    Component ids[] = { components... };
    Archetype ret;
    for (Component id : ids) {
        ret.set(id);
    }
    return ret;
}

Entity ECS::createEntity() {
    return m_entityManager->createEntity();
}

void ECS::destroyEntity(Entity e) {
    // deallocate entity ID for future use
    m_entityManager->destroyEntity(e);
    // delete component data
    m_componentManager->notifyDestroyed(e);
    // remove entity from all systems
    m_systemManager->notifyDestroyed(e);
}

template<typename T>
void ECS::registerComponent() {
    m_componentManager->registerComponent<T>();
}

template<typename T>
void ECS::addComponents(Entity e, T component) {
    m_componentManager->addComponent<T>(e, component);
    
    // update entity's archetype
    Archetype archetype = m_entityManager->getArchetype(e);
    archetype.set(m_componentManager->getId<T>());
    m_entityManager->setArchetype(e, archetype);

    // notify system manager of change
    m_systemManager->notifyChanged(e, archetype);
}

template<typename T, typename... Ts>
void ECS::addComponents(Entity e, T component, Ts... components) {
    m_componentManager->addComponent<T>(e, component);
    
    // update entity's archetype
    Archetype archetype = m_entityManager->getArchetype(e);
    archetype.set(m_componentManager->getId<T>());
    m_entityManager->setArchetype(e, archetype);

    // recursive call
    addComponents(e, components...);
}

template<typename T>
void ECS::removeComponent(Entity e) {
    m_componentManager->removeComponent<T>(e);
    
    // update entity's archetype
    Archetype archetype = m_entityManager->getArchetype(e);
    archetype.reset(m_componentManager->getId<T>());
    m_entityManager->setArchetype(e, archetype);

    // notify system manager of change
    m_systemManager->notifyChanged(e, archetype);
}

template<typename T>
T &ECS::getComponent(Entity e) {
    return m_componentManager->getComponent<T>(e);
}

template<typename T>
Component ECS::getComponentId() {
    return m_componentManager->getId<T>();
}

template<typename T>
std::shared_ptr<T> ECS::registerSystem(Archetype archetype) {
    return m_systemManager->registerSystem<T>(archetype);
}

template<typename T>
void ECS::setSystemArchetype(Archetype archetype) {
    m_systemManager->setArchetype<T>(archetype);
}

void ECS::updateSystems(unsigned int dt) {
    m_systemManager->update(dt);
}

