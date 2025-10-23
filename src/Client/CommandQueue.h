#pragma once

#include <functional>
#include <mutex>

class CommandQueue {
private:
    std::mutex mutex;
    std::vector<std::function<void()>> commands;
public:
    void Push(std::function<void()> cmd) {
        std::lock_guard<std::mutex> lock(mutex);
        commands.push_back(std::move(cmd));
    }

    void ExecuteAll() {
        std::vector<std::function<void()>> copy;
        {
            std::lock_guard<std::mutex> lock(mutex);
            copy.swap(commands);
        }
        for (auto& cmd : copy) cmd();
    }
};
