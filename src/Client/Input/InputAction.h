#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <SDL2/SDL.h>
#include "../../Core/Connection.h"

class InputAction {
private:
    std::string name;
    std::vector<SDL_Keycode> boundKeys;
    int priority;

public:
    Signal<> Pressed;
    Signal<> Released;

    explicit InputAction(std::string n, int p = 0)
        : name(std::move(n)), priority(p) {}

    const std::string& GetName() const { return name; }

    void AddKey(SDL_Keycode key) {
        if (!Matches(key))
            boundKeys.push_back(key);
    }

    void RemoveKey(SDL_Keycode key) {
        boundKeys.erase(std::remove(boundKeys.begin(), boundKeys.end(), key), boundKeys.end());
    }

    bool Matches(SDL_Keycode key) const {
        return std::find(boundKeys.begin(), boundKeys.end(), key) != boundKeys.end();
    }

    const std::vector<SDL_Keycode>& GetBoundKeys() const { return boundKeys; }

    int GetPriority() const { return priority; }
    void SetPriority(int p) { priority = p; }
};