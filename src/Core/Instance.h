#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include "../Util/UUID.hpp"
#include "Connection.h"
#include "../Common/Network/PacketCodec.h"

class Component;
class World;

using Util::UUID;

class Instance {
protected:
    using Tag = std::string;
    std::vector<Tag> tags;

    UUID uuid;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
    std::unordered_map<std::type_index, bool> locked;
    World* world;

    bool destroyed = false;
    bool dirty = false;
public:
    Signal<> Destroyed;

    Instance(): uuid(UUID::random()) {}
    Instance(const UUID& id): uuid(id) {}
    virtual ~Instance();

    void AddTag(const Tag& tag) {
        tags.push_back(tag);
    }

    bool HasTag(const Tag& tag) const {
        for (const auto& t : tags) {
            if (t == tag) return true;
        }
        return false;
    }

    void RemoveTag(const Tag& tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    }
    
    virtual void Tick(float dt);
    UUID GetUUID() const { return uuid; }
    const std::vector<Tag>& GetTags() const { return tags; }

    virtual bool operator==(const Instance& other) const {
        return uuid == other.uuid;
    }

    void SetWorld(World* w);
    World* GetWorld() const;

    virtual void Destroy();

    bool IsDestroyed() const { return destroyed; }

    const std::unordered_map<std::type_index, std::shared_ptr<Component>>& GetAllComponents() const {
        if (destroyed) {
            static std::unordered_map<std::type_index, std::shared_ptr<Component>> empty;
            return empty;
        }
        return components;
    }

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args);

    template <typename T>
    T* GetComponent() const;

    template <typename T>
    bool HasComponent() const;

    template <typename T>
    void RemoveComponent();

    template <typename T>
    void LockComponent();

    virtual void Encode(PacketCodec& codec) const;
    virtual void Decode(PacketCodec& codec);

    virtual std::string Dump() const;

    virtual void SetDirty(bool val = true) {
        dirty = val;
    }

    void ClearDirty() { dirty = false; }
    bool IsDirty() { return dirty; }
};

#include "Instance.inl"