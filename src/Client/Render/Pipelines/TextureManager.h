#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include "Core/Resources/ResourceLocation.h"

class TextureManager {
private:
    SDL_Renderer* renderer;
    std::unordered_map<ResourceLocation, SDL_Texture*, ResourceLocation::Hash> textures;

public:
    explicit TextureManager(SDL_Renderer* renderer)
        : renderer(renderer) {}

    ~TextureManager() {
        UnloadAll();
    }

    SDL_Texture* GetTexture(const ResourceLocation& location) {
        // Check if already loaded
        auto it = textures.find(location);
        if (it != textures.end()) {
            return it->second;
        }

        // Construct a filesystem path for loading
        std::string filePath = "assets/" + location.GetTexturePath() + ".png";

        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load texture: " << filePath << " — " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "Failed to create texture: " << filePath << " — " << SDL_GetError() << std::endl;
            return nullptr;
        }

        // Cache it
        textures[location] = texture;
        return texture;
    }

    void Unload(const ResourceLocation& location) {
        auto it = textures.find(location);
        if (it != textures.end()) {
            SDL_DestroyTexture(it->second);
            textures.erase(it);
        }
    }

    void UnloadAll() {
        for (auto& pair : textures) {
            SDL_DestroyTexture(pair.second);
        }
        textures.clear();
    }
};
