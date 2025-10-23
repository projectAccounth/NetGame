#pragma once
#include "Component.h"
#include "EntityAttributesComponent.h"
#include "HealthComponent.h"
#include "HitboxComponent.h"
#include "PhysicalPropertiesComponent.h"
#include "TransformComponent.h"

class ComponentRegistry {
    using Factory = std::function<std::unique_ptr<Component>()>;

    static std::unordered_map<std::string, Factory>& registry() {
        static std::unordered_map<std::string, Factory> inst;
        return inst;
    }

    static std::unordered_map<std::type_index, std::string>& reverse() {
        static std::unordered_map<std::type_index, std::string> inst;
        return inst;
    }

public:
    template<typename T>
    static void Register(const std::string& name) {
        registry()[name] = []() { return std::make_unique<T>(); };
        reverse()[std::type_index(typeid(T))] = name;
    }

    static void RegStatic() {
        Register<TransformComponent>("TransformComponent");
        Register<HealthComponent>("HealthComponent");
        Register<HitboxComponent>("HitboxComponent");
        Register<PhysicalPropertiesComponent>("PhysicalPropertiesComponent");
        Register<EntityAttributesComponent>("EntityAttributesComponent");
    }

    static std::unique_ptr<Component> Create(const std::string& name) {
        auto it = registry().find(name);
        if (it != registry().end()) {
            return it->second();
        }
        return nullptr;
    }

    static std::string GetName(const std::type_index& type) {
        auto it = reverse().find(type);
        if (it != reverse().end()) return it->second;
        return "Unknown";
    }

    static std::string RegDump() {
        std::string dump = "Registered Components:\n";
        for (const auto& [name, _] : registry()) {
            dump += " - " + name + "\n";
        }
        return dump;
    }
};
