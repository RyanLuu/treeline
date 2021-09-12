#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "core/entity.h"

class System {
 public:
    virtual ~System() = default;
    std::unordered_set<Entity> m_entities;
    virtual void update(uint64_t dt) = 0;
};

class SystemManager {
 public:
    template <typename T>
    std::shared_ptr<T> registerSystem(Archetype archetype) {
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

    template <typename T>
    void setArchetype(Archetype archetype) {
        static_assert(std::is_base_of<System, T>::value, "T must inherit from System");

        std::size_t typeHash = typeid(T).hash_code();
        const auto &pair = std::find_if(m_systems.begin(), m_systems.end(), [typeHash](const auto &pair) {
            return typeHash == typeid(*pair.first).hash_code();
        });
        assert(pair != m_systems.end());
        pair.second = archetype;
    }

    void notifyDestroyed(Entity e) {
        for (const auto &pair : m_systems) {
            pair.first->m_entities.erase(e);
        }
    }

    void notifyChanged(Entity e, Archetype archetype) {
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

    void update(uint64_t dt) {
        for (const auto &pair : m_systems) {
            pair.first->update(dt);
        }
    }

    template <typename T>
    bool hasSystem() {
        std::size_t typeHash = typeid(T).hash_code();
        const auto &pair = std::find_if(m_systems.begin(), m_systems.end(), [typeHash](const auto &pair) {
            return typeHash == typeid(*pair.first).hash_code();
        });
        return pair != m_systems.end();
    }

 private:
    std::unordered_map<std::shared_ptr<System>, Archetype> m_systems;
};
