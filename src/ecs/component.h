#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "./entity.h"

using Component = std::uint8_t;

class IComponentData {
 public:
    virtual ~IComponentData() = default;
    virtual void notifyDestroyed(Entity e) = 0;
};

template <typename T>
class ComponentData : public IComponentData {
 public:
    void insert(Entity e, T component) {
        assert(m_entityToIndex.find(e) == m_entityToIndex.end());
        size_t index = m_size;
        m_entityToIndex[e] = index;
        m_indexToEntity[index] = e;
        m_data[index] = component;
        m_size++;
    }

    void remove(Entity e) {
        assert(m_entityToIndex.find(e) != m_entityToIndex.end());
        size_t indexRemove = m_entityToIndex[e];
        size_t indexLast = m_size - 1;
        size_t entityLast = m_indexToEntity[indexLast];

        m_data[indexRemove] = m_data[indexLast];
        m_indexToEntity[indexRemove] = entityLast;
        m_entityToIndex[entityLast] = indexRemove;

        m_entityToIndex.erase(e);
        m_indexToEntity.erase(indexLast);

        m_size--;
    }

    T &get(Entity e) {
        assert(m_entityToIndex.find(e) != m_entityToIndex.end());
        return m_data[m_entityToIndex[e]];
    }
    void notifyDestroyed(Entity e) override {
        if (m_entityToIndex.find(e) != m_entityToIndex.end()) {
            remove(e);
        }
    }

 private:
    std::array<T, MAX_ENTITIES> m_data;
    std::unordered_map<Entity, size_t> m_entityToIndex;
    std::unordered_map<size_t, Entity> m_indexToEntity;
    size_t m_size;
};

class ComponentManager {
 public:
    template <typename T>
    void registerComponent() {
        assert(m_numComponents < MAX_COMPONENTS);
        std::size_t typeHash = typeid(T).hash_code();
        assert(m_idMap.find(typeHash) == m_idMap.end());
        m_idMap.insert({typeHash, m_numComponents});
        m_dataMap.insert({typeHash, std::make_shared<ComponentData<T>>()});
        m_numComponents++;
    }

    template <typename T>
    void addComponent(Entity e, T component) {
        getComponentData<T>()->insert(e, component);
    }

    template <typename T>
    void removeComponent(Entity e) {
        getComponentData<T>()->remove(e);
    }

    template <typename T>
    T &getComponent(Entity e) {
        return getComponentData<T>()->get(e);
    }

    template <typename T>
    Component getId() {
        std::size_t typeHash = typeid(T).hash_code();
        assert(m_idMap.find(typeHash) != m_idMap.end());  // Component is not registered
        return m_idMap[typeHash];
    }

    void notifyDestroyed(Entity e) {
        for (const auto &pair : m_dataMap) {
            pair.second->notifyDestroyed(e);
        }
    }

 private:
    template <typename T>
    std::shared_ptr<ComponentData<T>> getComponentData() {
        std::size_t typeName = typeid(T).hash_code();
        assert(m_idMap.find(typeName) != m_idMap.end());
        return std::static_pointer_cast<ComponentData<T>>(m_dataMap[typeName]);
    }
    std::unordered_map<std::size_t, std::shared_ptr<IComponentData>> m_dataMap;
    std::unordered_map<std::size_t, Component> m_idMap;
    size_t m_numComponents;
};
