#pragma once
#include "../../Core/Connection.h"
#include "Packet.h"
#include "Util/UUID.hpp"
#include <unordered_map>
#include <typeindex>
#include <functional>

class PacketDispatcher {
private:
    std::unordered_map<std::type_index, std::function<void(Packet&, const Util::UUID&)>> handlers;
    std::unordered_map<std::type_index, std::shared_ptr<void>> signals; // Store signals generically

public:
    template<typename T>
    void Register() {
        static_assert(std::is_base_of_v<Packet, T>);
        
        // Create and store the signal for type T
        auto signal = std::make_shared<Signal<T&, const Util::UUID&>>();
        signals[typeid(T)] = signal;

        handlers[typeid(T)] = [signal](Packet& pkt, const Util::UUID& id) {
            signal->Fire(static_cast<T&>(pkt), id);
        };
    }

    template<typename T>
    Signal<T&, const Util::UUID&>& GetSignal() {
        auto it = signals.find(typeid(T));
        if (it == signals.end()) {
            throw std::runtime_error("Signal for this packet type is not registered.");
        }
        return *std::static_pointer_cast<Signal<T&, const Util::UUID&>>(it->second);
    }

    void Dispatch(Packet& packet, const Util::UUID& clientId) {
        auto it = handlers.find(typeid(packet));
        if (it != handlers.end()) {
            it->second(packet, clientId);
        }
    }
};