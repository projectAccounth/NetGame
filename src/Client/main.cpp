#define SDL_MAIN_HANDLED
#include "Client.h"
#include "Utils/CommandParser.h"

int main() {
    auto& client = Client::GetInstance();
    client.Initialize();
    client.StartNetwork();

    CommandParser parser;

    // Register commands
    parser.RegisterCommand("/connect", [&client](const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: /connect <host> [port]\n";
            return;
        }
        std::string host = args[1];
        unsigned short port = 4000;
        if (args.size() >= 3) {
            try {
                port = static_cast<unsigned short>(std::stoul(args[2]));
            } catch (...) {
                std::cout << "Invalid port, using default " << port << "\n";
            }
        }
        std::cout << "Connecting to " << host << ":" << port << "...\n";
        client.RequestConnect(host, port);
    });

    parser.RegisterCommand("/name", [&client](const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: /name <username>\n";
            return;
        }
        auto username = args[1];
        std::cout << "Username set to '" << username << "'\n";
        client.SetUsername(username);
    });

    parser.RegisterCommand("/quit", [](const std::vector<std::string>&) {
        std::cout << "Quitting...\n";
        exit(0);
    });

    std::cout << "Type messages to chat. Commands: /connect <host> [port], /name <username>, /quit\n";

    // Input loop
    std::string input;
    while (true) {
        if (!std::getline(std::cin, input)) break;

        // Trim leading/trailing whitespace
        input.erase(input.begin(), std::find_if(input.begin(), input.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        input.erase(std::find_if(input.rbegin(), input.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), input.end());

        if (input.empty()) continue;

        if (input.front() == '/') {
            parser.ParseCommand(input);
        } else {
            // Normal chat message
            ChatMessagePacket chat;
            chat.message = input;
            chat.sender = client.GetUsername();
            client.SendPacket(chat);
        }
    }

    client.Shutdown();
    return 0;
}