#pragma once
#include "Components/Component.h"
#include "Instance.h"

template <typename T, typename... Args>
T& Instance::AddComponent(Args&&... args) {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    std::type_index typeIdx(typeid(T));

    if (locked[typeIdx]) {
        throw std::runtime_error("[Instance] Component of this type is locked and cannot be added.");
    }

    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T& ref = *comp;

    comp->OnAttach(this);
    components[typeIdx] = std::move(comp);
    
    return ref;
}

template <typename T>
T* Instance::GetComponent() const {
    std::type_index typeIdx(typeid(T));
    auto it = components.find(typeIdx);
    if (it != components.end())
        return static_cast<T*>(it->second.get());
    return nullptr;
}

template <typename T>
void Instance::RemoveComponent() {
    std::type_index typeIdx(typeid(T));

    if (locked[typeIdx] && !destroyed) {
        throw std::runtime_error("[Instance] Component of this type is locked and cannot be removed.");
    }
    
    auto it = components.find(typeIdx);
    if (it != components.end()) {
        it->second->OnDetach();
        components.erase(it);
    }
}

template <typename T>
bool Instance::HasComponent() const {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    return GetComponent<T>() != nullptr;
}

template <typename T>
void Instance::LockComponent() {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    locked[typeid(T)] = true;
}
