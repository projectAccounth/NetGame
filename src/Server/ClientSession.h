#pragma once
#include "../Common/NetCommon.h"
#include "../Util/UUID.hpp"
#include <cstdint>
#include <functional>

using Util::UUID;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    std::function<void(UUID, const std::vector<uint8_t>&)> onMessage;
    std::function<void(UUID)> onDisconnect;

    ClientSession(tcp::socket socket, UUID id)
        : socket(std::move(socket)), uuid(std::move(id)), isConnected(true) {}

    void start() {
        std::cout << "[Session] Started for " << uuid << std::endl;
        readLoop();
    }

    bool send(const std::vector<uint8_t>& msg) {
        if (!isConnected) {
            std::cerr << "[Session] Cannot send message, client " << uuid << " is disconnected.\n";
            return false;
        }

        asio::post(socket.get_executor(), [self = shared_from_this(), msg]() {
            bool busy = !self->outgoing.empty();
            self->outgoing.push_back(msg);
            if (!busy) self->doWrite();
        });

        return true; // Message successfully queued for sending
    }

    void stop() {
        asio::post(socket.get_executor(), [self = shared_from_this()]() {
            self->isConnected = false;
            self->socket.close();
        });
    }

private:
    void doWrite() {
        asio::async_write(socket,
            asio::buffer(outgoing.front()),
            [self = shared_from_this()](std::error_code ec, std::size_t) {
                if (!ec) {
                    self->outgoing.erase(self->outgoing.begin());
                    if (!self->outgoing.empty()) self->doWrite();
                } else {
                    std::cerr << "[Session] Write error for client " << self->uuid << ": " << ec.message() << "\n";
                    self->isConnected = false;
                }
            });
    }

    void readLoop() {
        socket.async_read_some(asio::buffer(buffer),
            [self = shared_from_this()](std::error_code ec, std::size_t len) {
                if (!ec) {
                    std::vector<uint8_t> msg(self->buffer.data(), self->buffer.data() + len);
                    if (self->onMessage)
                        self->onMessage(self->uuid, msg);
                    self->readLoop();
                } else {
                    std::cout << "[Session] " << self->uuid << " disconnected\n";
                    self->isConnected = false;
                    if (self->onDisconnect)
                        self->onDisconnect(self->uuid);
                }
            });
    }

    tcp::socket socket;
    std::array<char, 2048> buffer; // 2KB buffer for larger payloads
    std::vector<std::vector<uint8_t>> outgoing;
    UUID uuid;
    bool isConnected; // Tracks whether the client is still connected
};