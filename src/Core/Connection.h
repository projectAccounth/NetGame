#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

//
// Connection: RAII-safe connection handle.
//
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

class Connection {
private:
    std::weak_ptr<bool> valid_flag;
    std::function<void()> disconnect_func;
    bool auto_disconnect = true;

public:
    Connection() = default;
    Connection(std::weak_ptr<bool> valid, std::function<void()> func, bool auto_disconnect = true)
        : valid_flag(std::move(valid)), disconnect_func(std::move(func)), auto_disconnect(auto_disconnect) {}

    // Move-only
    Connection(Connection&& other) noexcept
        : valid_flag(std::move(other.valid_flag)),
          disconnect_func(std::move(other.disconnect_func)),
          auto_disconnect(other.auto_disconnect) {
        other.auto_disconnect = false;
    }

    Connection& operator=(Connection&& other) noexcept {
        if (this != &other) {
            Disconnect();
            valid_flag = std::move(other.valid_flag);
            disconnect_func = std::move(other.disconnect_func);
            auto_disconnect = other.auto_disconnect;
            other.auto_disconnect = false;
        }
        return *this;
    }

    void Disconnect() {
        if (auto flag = valid_flag.lock()) {
            if (*flag && disconnect_func) {
                disconnect_func();
                *flag = false;
            }
        }
    }

    void Detach() { auto_disconnect = false; }

    ~Connection() {
        if (auto_disconnect) {
            Disconnect();
        }
    }

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
};


//
// Signal: strongly typed, lightweight, event dispatcher.
//
template <typename... Args>
class Signal {
private:
    using Slot = std::function<void(Args...)>;
    struct Listener {
        size_t id;
        Slot callback;
        std::shared_ptr<bool> valid;
    };

    std::vector<Listener> listeners;
    size_t next_id = 0;

public:
    Signal() = default;
    ~Signal() = default;

    // Normal connection (RAII)
    Connection Connect(Slot callback) {
        return AddListener(std::move(callback), true);
    }

    // Persistent connection (won’t auto-disconnect)
    Connection ConnectPersistent(Slot callback) {
        return AddListener(std::move(callback), false);
    }

    // One-shot normal
    Connection ConnectOnce(Slot callback) {
        return AddListenerOnce(std::move(callback), true);
    }

    // One-shot persistent
    Connection ConnectOncePersistent(Slot callback) {
        return AddListenerOnce(std::move(callback), false);
    }

    // Emit (Fire): notify all listeners
    void Fire(Args... args) {
        for (auto& listener : listeners) {
            if (*listener.valid) {
                listener.callback(args...);
            }
        }
    }

    void Disconnect(size_t id) {
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
            [id](const Listener& l) { return l.id == id; }),
            listeners.end());
    }

    void DisconnectAll() {
        listeners.clear();
    }

private:
    Connection AddListener(Slot callback, bool auto_disconnect) {
        auto valid_flag = std::make_shared<bool>(true);
        const size_t id = next_id++;
        listeners.push_back({ id, std::move(callback), valid_flag });
        return Connection(valid_flag, [this, id]() { Disconnect(id); }, auto_disconnect);
    }

    Connection AddListenerOnce(Slot callback, bool auto_disconnect) {
        auto valid_flag = std::make_shared<bool>(true);
        const size_t id = next_id++;
        listeners.push_back({
            id,
            [this, id, callback](Args... args) {
                callback(args...);
                Disconnect(id);
            },
            valid_flag
        });
        return Connection(valid_flag, [this, id]() { Disconnect(id); }, auto_disconnect);
    }
};
