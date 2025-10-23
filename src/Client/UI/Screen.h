#pragma once

#include <SDL2Gui/lfmain>
#include <vector>

class Screen {
protected:
    std::vector<GUILib::PGuiObject> children;
    SDL_Renderer* renderer;

    bool initialized = false;
public:
    Screen() = default;

    virtual void Initialize() {
        initialized = true;
    }

    void SetRenderer(SDL_Renderer* renderer) {
        for (auto& child : children) {
            if (child) child->updateRenderer(renderer);
        }
        this->renderer = renderer;
    }

    void AddChild(GUILib::PGuiObject child) {
        children.push_back(child);
        child->updateRenderer(renderer);
    }

    void RemoveChild(GUILib::PGuiObject child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    const std::vector<GUILib::PGuiObject>& GetChildren() const {
        return children;
    }

    virtual void Render() {
        for (auto& child : children) {
            if (child->isVisible()) child->render();
        }
    }

    virtual void HandleEvent(const SDL_Event& event) {
        for (auto& child : children) {
            if (child->isVisible() && child->isActive()) child->handleEvent(event);
        }
    }

    bool IsInitialized() const {
        return initialized;
    }

    virtual ~Screen() = default;
};