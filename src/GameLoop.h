#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include "Core/World/World.h"
#include "Client/Render/RenderSystem.h"
#include "Client/CommandQueue.h"
#include "Client/Input/InputContext.h"
#include "Core/Components/TransformComponent.h"

class GameLoop {
private:
    World& world;
    RenderSystem& renderer;
    std::mutex worldMutex;
    CommandQueue commandQueue;
    std::atomic<bool> running{true};
    std::thread serverThread;

    std::vector<std::unique_ptr<InputContext>> contexts;

    float tickRate = 0.014f; // ~64hz

public:
    GameLoop(World& w, RenderSystem& r) : world(w), renderer(r) {}

    void Start() {
        StartServerThread();
        RunMainLoop();
    }

    void AddInputContext(std::unique_ptr<InputContext> ctx) {
        contexts.push_back(std::move(ctx));
    }

private:
    void StartServerThread() {
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
                    commandQueue.ExecuteAll();
                    world.Tick(deltaTime.count()); // Pass seconds as double
                }

                tickCount++;

                if (now - lastReportTime >= 1s) {
                    double tps = tickCount / duration_cast<duration<double>>(now - lastReportTime).count();
                    std::cout << "[Server] Real-Time Tick Rate: " << tps << " ticks/sec\n";

                    tickCount = 0;
                    lastReportTime = now;
                }

                std::this_thread::sleep_for(7ms);
            }

            std::cout << "[Server] Stopped.\n";
        });
    }

    void RunMainLoop() {
        SDL_Event event;
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
            }

            {
                std::lock_guard<std::mutex> lock(worldMutex);
                renderer.RenderWorld(world);
            }
        }

        running = false;
        if (serverThread.joinable())
            serverThread.join();
        std::cout << "[Main] Exiting.\n";
    }

public:
    CommandQueue& GetCommandQueue() { return commandQueue; }
};