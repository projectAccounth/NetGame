#pragma once
#include "Packet.h"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <functional>
#include <cstdint>
#include <stdexcept>

//
// PacketRegistry — handles mapping IDs to packet constructors.
//
class PacketRegistry {
private:
    using CreateFunc = std::function<std::unique_ptr<Packet>()>;

    std::unordered_map<uint32_t, CreateFunc> factories;
    std::unordered_map<std::type_index, uint32_t> type_to_id;

public:
    template<typename T>
    void Register(uint32_t id) {
        static_assert(std::is_base_of_v<Packet, T>, "T must inherit from Packet.");
        factories[id] = []() -> std::unique_ptr<Packet> { return std::make_unique<T>(); };
        type_to_id[typeid(T)] = id;
    }

    std::unique_ptr<Packet> Create(uint32_t id) const {
        auto it = factories.find(id);
        if (it == factories.end()) {
            throw std::runtime_error("Unknown packet ID: " + std::to_string(id));
        }
        return it->second();
    }

    template<typename T>
    uint32_t GetID() const {
        auto it = type_to_id.find(typeid(T));
        if (it == type_to_id.end()) {
            throw std::runtime_error("Packet type not registered.");
        }
        return it->second;
    }
};