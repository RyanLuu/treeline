
template <typename T>
Assets<T>::Assets() {
    static_assert(std::is_base_of<Asset, T>::value, "T must inherit from Asset");
}

template <typename T>
AssetId Assets<T>::load(const std::string &filepath) {
    std::shared_ptr<T> resource = std::make_shared<T>();
    resource->load(filepath);
    m_resources.insert({m_currentId, resource});
    return m_currentId++;
}

template <typename T>
std::shared_ptr<T> Assets<T>::get(AssetId id) {
    auto it = m_resources.find(id);
    if (it == m_resources.end()) {
        return nullptr;
    }
    return it->second;
}

template <typename T>
void Assets<T>::unload(AssetId id) {
    for (auto it = m_resources.begin(); it != m_resources.end(); it++) {
        if (it->first == id) {
            it->second->unload();
            m_resources.erase(it->first);
            return;
        }
    }
}

template <typename T>
void Assets<T>::unloadAll() {
    for (auto it = m_resources.begin(); it != m_resources.end(); it++) {
        it->second->unload();
    }
    m_resources.clear();
}

