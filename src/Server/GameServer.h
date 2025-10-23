#pragma once

#include "Common/NetCommon.h"
#include "Core/Connection.h"
#include "Common/Packets/2W/PlayerJoinPacket.h"
#include "Common/Packets/2W/PlayerLeavePacket.h"
#include "Common/Packets/2W/ChatMessagePacket.h"
#include "Common/Packets/S2C/HandshakeAckPacket.h"
#include "Common/Packets/C2S/HandshakePacket.h"
#include "Common/Network/PacketDispatcher.h"
#include "Common/Network/PacketIO.h"
#include "Common/Network/ProtocolRegistry.h"
#include "Core/World/ServerWorld.h"
#include "NetworkSystem.h"
#include "Core/World/World.h"
#include "Core/World/ServerWorld.h"
#include <memory>

using Util::UUID;

class GameServer {
private:
    asio::io_context io;
    NetworkSystem network;

    PacketDispatcher dispatcher;

    std::unique_ptr<PacketRegistry>& registry = network.registry;
    PacketIO& pIO = network.pIO;

    std::unordered_map<UUID, std::string> clients; // UUID to username map

    void handleClientConnect(UUID id) {
        std::cout << "[GameServer] New connection: " << id << std::endl;

        // Start handshake process
        HandshakePacket handshakeRequest;
        handshakeRequest.authToken = id;

        network.sendTo(id, pIO.EncodePacket(handshakeRequest));
    }

    void handleClientDisconnect(UUID id) {
        std::cout << "[GameServer] Client disconnected: " << id << std::endl;
        clients.erase(id);
        
        PlayerLeavePacket leave;
        leave.username = clients[id];
        leave.uuid = id;
        
        network.broadcast(pIO.EncodePacket(leave));
    }

    void handleClientMessage(UUID id, const std::vector<uint8_t>& msg) {
        std::cout << "[GameServer] Received message from " << id << ": " << msg.size() << " bytes\n";
        try {
            auto packet = pIO.DecodePacket(msg);
            if (packet) dispatcher.Dispatch(*packet);
        } 
        catch (const std::exception& e) {
            std::cerr << "[GameServer] Packet decode error: " << e.what() << std::endl;
        }
    }

    const uint32_t ServerProtocolVersion = 2;

    void handleHandshake(const HandshakePacket& packet, UUID clientID) {
        if (packet.clientVersion != ServerProtocolVersion) {
            std::cerr << "[GameServer] Client " << clientID << " has incompatible version: " << packet.clientVersion << "\n";
            HandshakeAckPacket ack{false, "Incompatible version"};
            network.sendTo(clientID, pIO.EncodePacket(ack));
            network.disconnectClient(clientID);
            return;
        }

        // Optionally verify authToken here...

        std::cout << "[GameServer] Client " << clientID << " verified successfully.\n";
        HandshakeAckPacket ack{true, "Welcome to the server!"};
        network.sendTo(clientID, pIO.EncodePacket(ack));
    }

    ServerWorld serverWorld = ServerWorld();
    std::thread serverThread;
    std::mutex worldMutex;
    std::atomic<bool> running{false};

public:
    explicit GameServer(unsigned short port)
        : network(io, port) {}

    void Initialize() {
        dispatcher.Register<PlayerJoinPacket>();
        dispatcher.Register<ChatMessagePacket>();
        dispatcher.Register<HandshakePacket>();
        dispatcher.Register<HandshakeAckPacket>();

        network.onClientConnect.ConnectPersistent([this](UUID id) {
            handleClientConnect(id);
        });

        network.onClientDisconnect.ConnectPersistent([this](UUID id) {
            handleClientDisconnect(id);
        });

        network.onClientMessage.ConnectPersistent([this](UUID id, const std::vector<uint8_t>& data) {
            handleClientMessage(id, data);
        });

        dispatcher.GetSignal<PlayerJoinPacket>().ConnectPersistent(
            [this](PlayerJoinPacket& pkt) {  });
        dispatcher.GetSignal<ChatMessagePacket>().ConnectPersistent(
            [this](ChatMessagePacket& pkt) { 
                std::cout << "[GameServer] Chat from " << pkt.sender << ": " << pkt.message << "\n";
                network.sendToAll(pIO.EncodePacket(pkt)); 
            });

        dispatcher.GetSignal<HandshakePacket>().ConnectPersistent([this](HandshakePacket& packet) {
            handleHandshake(packet, packet.authToken);
        });
    }

    void run() {
        running = true;

        serverThread = std::thread([this]() {
            using namespace std::chrono;
            using namespace std::chrono_literals;

            auto lastTickTime = steady_clock::now();
            auto lastReportTime = lastTickTime;
            int tickCount = 0;

            while (running) {
                auto now = steady_clock::now();
                duration<double> deltaTime = now - lastTickTime;
                lastTickTime = now;

                {
                    std::lock_guard<std::mutex> lock(worldMutex);
                    // commandQueue.ExecuteAll(); // execute player actions there
                    serverWorld.Tick(deltaTime.count()); // Pass seconds as double
                }

                tickCount++;

                if (now - lastReportTime >= 1s) {
                    double tps = tickCount / duration_cast<duration<double>>(now - lastReportTime).count();
                    // std::cout << "[Server] Real-Time Tick Rate: " << tps << " ticks/sec\n";

                    tickCount = 0;
                    lastReportTime = now;
                }

                std::this_thread::sleep_for(7ms);
            }

            std::cout << "[Server] Stopped.\n";
        });

        io.run();
    }

    void broadcast(const std::vector<uint8_t>& msg) {
        network.broadcast(msg);
    }

    void broadcast(const Packet& packet) {
        network.broadcast(packet);
    }

    void stop() {
        io.stop();

        running = false;
        if (serverThread.joinable())
            serverThread.join();
    }

    void disconnectClient(const UUID& id) {
        network.disconnectClient(id);
    }

    bool isClientConnected(const UUID& id) const {
        return clients.find(id) != clients.end();
    }

    std::string getClientUsername(const UUID& id) const {
        auto it = clients.find(id);
        return it != clients.end() ? it->second : "";
    }

    size_t getClientCount() const {
        return clients.size();
    }

    std::vector<UUID> getAllClientIDs() const {
        std::vector<UUID> ids;
        for (const auto& [id, _] : clients) {
            ids.push_back(id);
        }
        return ids;
    }

    std::vector<std::string> getAllUsernames() const {
        std::vector<std::string> names;
        for (const auto& [_, name] : clients) {
            names.push_back(name);
        }
        return names;
    }

    const PacketRegistry& getRegistry() const {
        return *registry;
    }

    ServerWorld& GetWorld() { return serverWorld; }
};