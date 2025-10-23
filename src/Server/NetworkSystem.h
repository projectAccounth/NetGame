#pragma once

#include "../Common/NetCommon.h"
#include "../Core/Connection.h"
#include "../Util/UUID.hpp"
#include "ClientSession.h"
#include "Common/Network/PacketIO.h"
#include "Common/Network/PacketRegistry.h"
#include "Common/Network/ProtocolRegistry.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>

using Util::UUID;

class NetworkSystem {
public:
    Signal<UUID> onClientConnect;
    Signal<UUID> onClientDisconnect;
    Signal<UUID, const std::vector<uint8_t>&> onClientMessage;

    std::unique_ptr<PacketRegistry> registry = ProtocolRegistry::Create();
    PacketIO pIO = PacketIO(*registry);

    NetworkSystem(asio::io_context& io, unsigned short port)
        : io(io), acceptor(io, tcp::endpoint(tcp::v4(), port)) {
        acceptNext();
        std::cout << "[NetworkSystem] Listening on port " << port << std::endl;
    }

    void broadcast(const std::vector<uint8_t>& msg, bool reliable = false) {
        for (auto& [id, session] : sessions)
            sendTo(id, msg, reliable);
    }

    void broadcast(const Packet& packet, bool reliable = false) {
        broadcast(pIO.EncodePacket(packet), reliable);
    }

    void sendTo(const UUID& id, const std::vector<uint8_t>& msg, bool reliable = false) {
        if (auto it = sessions.find(id); it != sessions.end()) {
            auto success = it->second->send(msg);
            if (!success && reliable) {
                queueRetry(id, msg);
            }
        }
    }

    void sendTo(const UUID& id, const Packet& packet, bool reliable = false) {
        sendTo(id, pIO.EncodePacket(packet), reliable);
    }

    void sendToAllExcept(const UUID& exclude_id, const std::vector<uint8_t>& msg, bool reliable = false) {
        for (auto& [id, session] : sessions) {
            if (id != exclude_id)
                sendTo(id, msg, reliable);
        }
    }

    void sendToAllExcept(const UUID& exclude_id, const Packet& packet, bool reliable = false) {
        sendToAllExcept(exclude_id, pIO.EncodePacket(packet), reliable);
    }

    void sendToAllExcept(const std::vector<UUID>& exclude_ids, const std::vector<uint8_t>& msg, bool reliable = false) {
        for (auto& [id, session] : sessions) {
            if (std::find(exclude_ids.begin(), exclude_ids.end(), id) == exclude_ids.end())
                sendTo(id, msg, reliable);
        }
    }

    void sendToAllExcept(const std::vector<UUID>& exclude_ids, const Packet& packet, bool reliable = false) {
        sendToAllExcept(exclude_ids, pIO.EncodePacket(packet), reliable);
    }

    void sendToAll(const std::vector<uint8_t>& msg, bool reliable = false) {
        for (const auto& [id, session] : sessions) {
            sendTo(id, msg, reliable);
        }
    }

    void sendToAll(const Packet& packet, bool reliable = false) {
        sendToAll(pIO.EncodePacket(packet), reliable);
    }

    void disconnectClient(const UUID& id) {
        if (auto it = sessions.find(id); it != sessions.end()) {
            it->second->stop();
            sessions.erase(it);
            std::cout << "[NetworkSystem] Client " << id << " disconnected by NetworkSystem\n";
            onClientDisconnect.Fire(id);
        }
    }

private:
    void acceptNext() {
        auto socket = std::make_shared<tcp::socket>(io);
        acceptor.async_accept(*socket, [this, socket](std::error_code ec) {
            if (!ec) {
                auto id = UUID::random();
                auto session = std::make_shared<ClientSession>(std::move(*socket), id);

                // Handle disconnection
                session->onDisconnect = [this](const UUID& id) {
                    sessions.erase(id);
                    std::cout << "[NetworkSystem] Client " << id << " removed\n";
                    onClientDisconnect.Fire(id);
                };

                // Handle incoming messages
                session->onMessage = [this](const UUID& id, const std::vector<uint8_t>& msg) {
                    onClientMessage.Fire(id, msg);
                };

                sessions[id] = session;
                std::cout << "[NetworkSystem] Client " << id << " connected\n";

                session->start();
                onClientConnect.Fire(id);
            }
            acceptNext();
        });
    }

    void queueRetry(const UUID& id, const std::vector<uint8_t>& msg) {
        std::thread([this, id, msg]() {
            constexpr int maxRetries = 5;
            constexpr std::chrono::milliseconds retryDelay(100);

            for (int i = 0; i < maxRetries; ++i) {
                std::this_thread::sleep_for(retryDelay);

                if (auto it = sessions.find(id); it != sessions.end()) {
                    if (it->second->send(msg)) {
                        std::cout << "[NetworkSystem] Reliable send succeeded for client " << id << "\n";
                        return;
                    }
                }
            }

            std::cerr << "[NetworkSystem] Reliable send failed after " << maxRetries << " attempts for client " << id << "\n";
        }).detach();
    }

private:
    asio::io_context& io;
    tcp::acceptor acceptor;
    std::unordered_map<UUID, std::shared_ptr<ClientSession>> sessions;
};