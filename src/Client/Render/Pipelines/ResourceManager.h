#pragma once

#include "TextureManager.h"
class ResourceManager {
private:
    TextureManager textureManager;
public:
    explicit ResourceManager(SDL_Renderer* renderer): textureManager(renderer) {}
    ~ResourceManager() = default;

    TextureManager& GetTextureManager() { return textureManager; }
};