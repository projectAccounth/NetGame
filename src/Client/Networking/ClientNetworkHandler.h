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
#include "NetworkClient.h"
#include <asio/executor_work_guard.hpp>
#include <exception>

class ClientNetworkHandler {
protected:
    asio::io_context io;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard{asio::make_work_guard(io)};
    NetworkClient network = NetworkClient(io);

    PacketDispatcher dispatcher;
    std::unique_ptr<PacketRegistry> registry = ProtocolRegistry::Create();
    PacketIO pIO = PacketIO(*registry);

    std::thread networkThread;

    void RegisterPackets() {
        dispatcher.Register<ChatMessagePacket>();
        dispatcher.Register<PlayerJoinPacket>();
        dispatcher.Register<PlayerLeavePacket>();
        dispatcher.Register<HandshakePacket>();
        dispatcher.Register<HandshakeAckPacket>();
    }

    void InitListeners() {
        dispatcher.GetSignal<PlayerJoinPacket>().ConnectPersistent([](PlayerJoinPacket& pkt, const UUID&) {
            std::cout << "Player joined: " << pkt.username << "\n";
        });

        dispatcher.GetSignal<PlayerLeavePacket>().ConnectPersistent([](PlayerLeavePacket& pkt, const UUID&) {
            std::cout << "Player left: " << pkt.username << "\n";
        });

        dispatcher.GetSignal<ChatMessagePacket>().ConnectPersistent([](ChatMessagePacket& pkt, const UUID&) {
            std::cout << "[" << pkt.sender << "]: " << pkt.message << "\n";
        });

        dispatcher.GetSignal<HandshakeAckPacket>().ConnectOncePersistent([this](HandshakeAckPacket& pkt, const UUID&) {
            if (pkt.success) {
                std::cout << "[Client] Successfully connected! " << pkt.message << '\n';
            } else {
                Disconnect();
                std::cout << "[Client] Connection error: " << pkt.message << '\n';
            }
        });

        dispatcher.GetSignal<HandshakePacket>().ConnectOncePersistent([this](HandshakePacket& pkt, const UUID&) {
            HandleHandshake(UUID::random());
        });
    }

public:
    ClientNetworkHandler() = default;
    virtual ~ClientNetworkHandler() {
        StopNetwork();
    }

    virtual void HandleConnectFailed(const std::string& message) = 0;
    virtual void HandleConnectSuccess() = 0;
    virtual void HandleHandshake(UUID token) = 0;

    virtual void Initialize() {
        RegisterPackets();
        InitListeners();

        network.OnConnectFailed.ConnectPersistent([this](const std::string& msg) {
            HandleConnectFailed(msg);
        });

        network.OnConnected.ConnectPersistent([this]() {
            HandleConnectSuccess();
        });

        network.OnDisconnected.ConnectOncePersistent([]() {
            std::cout << "[ClientNetworkHandler] Disconnected.\n";
        });

        network.OnServerMessage.ConnectPersistent([this](const std::vector<uint8_t>& data) {
            try {
                auto packet = pIO.DecodePacket(data);
                dispatcher.Dispatch(*packet, UUID::null());
            } catch (const std::exception& e) {
                std::cout << "[ClientNetworkHandler/OnServerMessage] Message failed: " << e.what() << "\n";
            }
        });
    }

    void Disconnect() {
        network.disconnect();
    }

    void SendPacket(const Packet& packet) {
        network.send(pIO.EncodePacket(packet));
    }

    bool IsConnected() const { return network.isConnected(); }

    virtual void StartNetwork() {
        if (!networkThread.joinable()) {
            networkThread = std::thread([this]() {
                try {
                    io.run();
                } catch (const std::exception& e) {
                    std::cerr << "[ClientNetworkHandler] io_context exception: " << e.what() << "\n";
                }
            });
        }
    }

    virtual void StopNetwork() {
        work_guard.reset(); // Allow io_context to stop
        io.stop();
        if (networkThread.joinable()) {
            networkThread.join();
        }
    }

    virtual void RequestConnect(const std::string& host, unsigned short port) {
        if (!networkThread.joinable())
            StartNetwork();
        network.connect(host, port);
    }

    template <typename T, typename... Args>
    void SendPacket(Args&&... args) {
        static_assert(std::is_base_of_v<Packet, T>, "T must derive from Packet");
        T packet(std::forward<Args>(args)...);
        SendPacket(packet);
    }

    template <typename T>
    void RegisterPacketHandler(std::function<void(T&)> handler) {
        static_assert(std::is_base_of_v<Packet, T>, "T must derive from Packet");
        dispatcher.Register<T>();
        dispatcher.GetSignal<T>().ConnectPersistent(handler);
    }
};
