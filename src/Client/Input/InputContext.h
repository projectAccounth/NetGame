#pragma once

#include "../CommandQueue.h"
#include "../../Core/Objects/GameObject.h"
#include "../../Core/Components/TransformComponent.h"
#include "InputAction.h"

class InputContext {
private:
    std::unordered_map<std::string, std::unique_ptr<InputAction>> actions;
    std::unordered_map<SDL_Keycode, std::vector<InputAction*>> keyToActions;

public:
    InputContext() = default;

    // Basic bind (default priority = 0)
    template <typename... Keys>
    void BindAction(const std::string& name, std::function<void(bool)> callback, Keys... keys) {
        BindWithPriority(name, 0, std::move(callback), keys...);
    }

    // Priority-aware bind
    template <typename... Keys>
    void BindWithPriority(const std::string& name, int priority, std::function<void(bool)> callback, Keys... keys) {
        InputAction* actPtr = nullptr;

        // Create or reuse
        if (actions.find(name) == actions.end()) {
            auto action = std::make_unique<InputAction>(name, priority);
            actPtr = action.get();
            actions[name] = std::move(action);
        } else {
            actPtr = actions[name].get();
            actPtr->SetPriority(priority);
        }

        // Connect pressed/released signals
        actPtr->Pressed.ConnectPersistent([callback]() { callback(true); });
        actPtr->Released.ConnectPersistent([callback]() { callback(false); });

        // Register keys
        (actPtr->AddKey(keys), ...);
        for (auto key : {keys...})
            keyToActions[key].push_back(actPtr);

        // Sort each key’s actions by descending priority
        for (auto key : {keys...}) {
            auto& vec = keyToActions[key];
            std::sort(vec.begin(), vec.end(), [](InputAction* a, InputAction* b) {
                return a->GetPriority() > b->GetPriority();
            });
        }
    }

    void HandleEvent(const SDL_Event& event) {
        if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) return;

        SDL_Keycode key = event.key.keysym.sym;
        auto it = keyToActions.find(key);
        if (it == keyToActions.end()) return;

        // Get highest-priority action for this key
        auto& vec = it->second;
        if (!vec.empty()) {
            InputAction* topAction = vec.front(); // already sorted
            if (event.type == SDL_KEYDOWN)
                topAction->Pressed.Fire();
            else
                topAction->Released.Fire();
        }
    }

    InputAction* GetAction(const std::string& name) {
        auto it = actions.find(name);
        return (it != actions.end()) ? it->second.get() : nullptr;
    }

    // --- Unbinding and clearing (same as before) ---
    void UnbindKey(const std::string& name, SDL_Keycode key) {
        auto it = actions.find(name);
        if (it == actions.end()) return;

        InputAction* act = it->second.get();
        act->RemoveKey(key);

        auto kt = keyToActions.find(key);
        if (kt != keyToActions.end()) {
            auto& vec = kt->second;
            vec.erase(std::remove(vec.begin(), vec.end(), act), vec.end());
            if (vec.empty())
                keyToActions.erase(kt);
        }
    }

    void UnbindAction(const std::string& name) {
        auto it = actions.find(name);
        if (it == actions.end()) return;

        InputAction* act = it->second.get();

        for (auto key : act->GetBoundKeys()) {
            auto kt = keyToActions.find(key);
            if (kt != keyToActions.end()) {
                auto& vec = kt->second;
                vec.erase(std::remove(vec.begin(), vec.end(), act), vec.end());
                if (vec.empty())
                    keyToActions.erase(kt);
            }
        }

        actions.erase(it);
    }

    void ClearAllBindings() {
        actions.clear();
        keyToActions.clear();
    }

    ~InputContext() = default;
};