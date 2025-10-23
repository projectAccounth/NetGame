#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>

class CommandParser {
public:
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;

    // Register a command with its handler
    void RegisterCommand(const std::string& command, CommandHandler handler) {
        commands[command] = std::move(handler);
    }

    // Parse and execute a command
    void ParseCommand(const std::string& input) {
        auto args = splitArgs(input);
        if (args.empty()) return;

        const std::string& command = args[0];
        auto it = commands.find(command);
        if (it != commands.end()) {
            it->second(args); // Call the registered handler
        } else {
            std::cout << "Unknown command: " << command << "\n";
        }
    }

private:
    std::unordered_map<std::string, CommandHandler> commands;

    // Utility to split input into arguments
    static std::vector<std::string> splitArgs(const std::string& s) {
        std::istringstream iss(s);
        std::vector<std::string> out;
        std::string token;
        while (iss >> token) out.push_back(token);
        return out;
    }
};