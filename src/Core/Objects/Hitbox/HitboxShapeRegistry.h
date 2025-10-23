#pragma once

#include "HitboxShape.h"
#include <functional>
#include <memory>
#include <typeindex>

class HitboxShapeRegistry {
    using Factory = std::function<std::unique_ptr<HitboxShape>()>;

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
        Register<RectShape>("RectShape");
        Register<CircleShape>("CircleShape");
        Register<PolygonShape>("PolygonShape");
    }

    static std::unique_ptr<HitboxShape> Create(const std::string& name) {
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
        std::string dump = "Registered shapes:\n";
        for (const auto& [name, _] : registry()) {
            dump += " - " + name + "\n";
        }
        return dump;
    }
};
