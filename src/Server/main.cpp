#include "GameServer.h"
#include "NetworkSystem.h"
#include "../Common/Network/PacketIO.h"
#include "../Common/Network/ProtocolRegistry.h"
#include "../Common/Network/PacketDispatcher.h"

#include <string>
#include <algorithm>
#include <cctype>

bool is_empty_or_whitespace(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

int main() {
    try {
        GameServer server(4000);
        server.Initialize();

        std::thread console([&]() {
            std::string line;
            while (std::getline(std::cin, line)) {
                if (line == "/quit") break;
                if (line == "/list") {
                    std::cout << "[Server] Currently connected clients:\n";
                    
                    continue;
                }
                ChatMessagePacket chat;
                chat.message = line + "\n";
                chat.sender = "Server";
                server.broadcast(chat);
            }
            server.stop();
        });

        server.run();
        console.join();
    } catch (std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
    }
}