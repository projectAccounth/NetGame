#pragma once
#include "InputContext.h"

class InputService {
private:
    struct ContextEntry {
        std::shared_ptr<InputContext> context;
        int priority;
    };

    std::vector<ContextEntry> contexts;
    bool enabled = true;

public:

    InputService() = default;

    // Delete all constructors/operators
    InputService(const InputService&) = delete;
    InputService& operator=(const InputService&) = delete;

    InputService(InputService&&) = delete;
    InputService& operator=(InputService&&) = delete;
    
    void SetEnabled(bool state) { enabled = state; }
    bool IsEnabled() const { return enabled; }

    // Register a context with a priority (higher = first to handle events)
    void RegisterContext(std::shared_ptr<InputContext> ctx, int priority = 0) {
        contexts.push_back({ ctx, priority });
        std::sort(contexts.begin(), contexts.end(),
                  [](const ContextEntry& a, const ContextEntry& b) {
                      return a.priority > b.priority;
                  });
    }

    // Remove a specific context (by pointer)
    void UnregisterContext(std::shared_ptr<InputContext> ctx) {
        contexts.erase(std::remove_if(contexts.begin(), contexts.end(),
            [&](const ContextEntry& e) { return e.context == ctx; }),
            contexts.end());
    }

    // Clear all contexts
    void ClearContexts() {
        contexts.clear();
    }

    // Dispatch SDL event to contexts in priority order
    void HandleEvent(const SDL_Event& event) {
        if (!enabled) return;

        for (auto& entry : contexts) {
            entry.context->HandleEvent(event);
        }
    }

    // Process all SDL events automatically
    void PollEvents(const SDL_Event& e) {
        HandleEvent(e);
    }

    ~InputService() = default;
};