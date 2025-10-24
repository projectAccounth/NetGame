#pragma once

#include "Common/Packets/C2S/HandshakePacket.h"
#include "Input/InputService.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <fstream>
#include <vector>
#include "../Core/Resources/ResourceLocation.h"
#include "Networking/ClientNetworkHandler.h"
#include "Networking/NetworkClient.h"
#include "UI/Screen.h"

// Singleton instance
class Client : public ClientNetworkHandler {
private:
    InputService inputService;
    std::unique_ptr<Screen> currentScreen;
    std::string username;

    Client() = default;
    ~Client() = default;

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;

    const uint32_t CLIENT_PROTOCOL_VERSION = 2;
public:
    InputService& GetInputService() { return inputService; }
    Screen& GetCurrentScreen() { return *currentScreen; }
    void SetCurrentScreen(std::unique_ptr<Screen> screen) { currentScreen = std::move(screen); }

    void HandleConnectFailed(const std::string& msg) override {
        std::cout << "[Client] Connect failed. " << msg << '\n';
    }

    void HandleHandshake(UUID token) override {
        HandshakePacket packet;
        packet.clientVersion = CLIENT_PROTOCOL_VERSION;
        packet.authToken = token;

        SendPacket(packet);
    }

    void HandleConnectSuccess() override {
        std::cout << "[Client] Successfully established connection with server\n";
    }
    
    void Initialize(
        ResourceLocation configFile = ResourceLocation::FromPath("config/client.cfg"), 
        ResourceLocation keySettings = ResourceLocation::FromPath("config/keybinds.cfg")) {

        ClientNetworkHandler::Initialize();
        
        std::fstream keybinds(keySettings.GetRawDataPath());

        // format: CONFIG_KEY=VALUE
        std::string line;
        while (std::getline(keybinds, line)) {
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
        }
    }

    void SetUsername(const std::string& name) { 
        username = name; 
    }
    std::string GetUsername() const { return username; }

    void Shutdown() {
        StopNetwork();
        // Cleanup resources, save state, etc.
    }

    static Client& GetInstance() {
        static Client instance;
        return instance;
    }
};