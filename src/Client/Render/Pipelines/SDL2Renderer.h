#pragma once
#include "IRenderer.h"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <iostream>

#include "Core/Components/TransformComponent.h"
#include "Core/Objects/GameObject.h"
#include "Core/Components/HitboxComponent.h"
#include "Core/Resources/ResourceLocation.h"
#include "Util/GMath.h"

class SDL2Renderer : public IRenderer {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width;
    int height;
    std::string windowTitle;
public:
    SDL2Renderer(int w = 800, int h = 600) : width(w), height(h) {}
    virtual ~SDL2Renderer() { Shutdown(); }

    void Initialize() override {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        window = SDL_CreateWindow(windowTitle.c_str(),
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  SDL_WINDOW_SHOWN);
        if (!window) throw std::runtime_error("SDL_CreateWindow failed");

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) throw std::runtime_error("SDL_CreateRenderer failed");

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
    }

    void LoadTexture(const ResourceLocation& resourceLocation) {

    }

    void ClearFrame() override {
        SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    void DrawGameObject(const GameObject& obj) override {
        auto* transform = obj.GetComponent<TransformComponent>();
        if (!transform) return;

        Vector2d pos = transform->GetPosition();
        const auto& hitbox = *obj.GetHitbox();

        for (auto& h: hitbox.GetHitboxes()) {
            DrawShape(h.shape, pos);
        }
    }

    void DrawShape(const std::unique_ptr<HitboxShape>& shape, Vector2d worldPos) {
        if (auto rectShape = dynamic_cast<RectShape*>(shape.get())) {
            auto serverBounds = rectShape->GetBounds().Translated(worldPos);
            SDL_FRect rect;
            rect.x = serverBounds.x;
            rect.y = serverBounds.y;
            rect.w = serverBounds.width;
            rect.h = serverBounds.height;

            SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
            SDL_RenderFillRectF(renderer, &rect);
        }
        else if (auto circleShape = dynamic_cast<CircleShape*>(shape.get())) {
            auto serverPos = circleShape->GetCenter() + worldPos;
            DrawCircle(serverPos, circleShape->GetRadius(), { 255, 0, 0, 255 });
        }
    }

    void PresentFrame() override {
        SDL_RenderPresent(renderer);
    }

    void SetTitle(const std::string& title) override {
        windowTitle = title;
        if (window) SDL_SetWindowTitle(window, title.c_str());
    }

    void DrawLine(Vector2f from, Vector2f to, SDL_Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(renderer, static_cast<int>(from.x), static_cast<int>(from.y),
                           static_cast<int>(to.x), static_cast<int>(to.y));
    }

    void DrawCircle(Vector2f center, float radius, SDL_Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int cx = static_cast<int>(center.x);
        int cy = static_cast<int>(center.y);
        int r = static_cast<int>(radius);
        for (int w = 0; w < r * 2; w++) {
            for (int h = 0; h < r * 2; h++) {
                int dx = r - w; // horizontal offset
                int dy = r - h; // vertical offset
                if ((dx*dx + dy*dy) <= (r * r)) {
                    SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }

    void DrawTextSmooth(const std::string& text, Vector2f position, int fontSize) override {
        // Placeholder: Implement text rendering with SDL_ttf or similar
        std::cout << "DrawTextSmooth: " << text << " at (" << position.x << ", " << position.y << ")\n";
    }

    void DrawTextFixed(const std::string& text, Vector2f position, int fontSize) override {
        // Placeholder: Implement text rendering with SDL_ttf or similar
        std::cout << "DrawTextFixed: " << text << " at (" << position.x << ", " << position.y << ")\n";
    }

    void DrawRect(Vector2f topLeft, Vector2f rightDown, SDL_Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_FRect rect;
        rect.x = topLeft.x;
        rect.y = topLeft.y;
        rect.w = rightDown.x - topLeft.x;
        rect.h = rightDown.y - topLeft.y;
        SDL_RenderDrawRectF(renderer, &rect);
    }



    void DrawTriangle(Vector2f p1, Vector2f p2, Vector2f p3, SDL_Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(renderer, static_cast<int>(p1.x), static_cast<int>(p1.y),
                           static_cast<int>(p2.x), static_cast<int>(p2.y));
        SDL_RenderDrawLine(renderer, static_cast<int>(p2.x), static_cast<int>(p2.y),
                           static_cast<int>(p3.x), static_cast<int>(p3.y));
        SDL_RenderDrawLine(renderer, static_cast<int>(p3.x), static_cast<int>(p3.y),
                           static_cast<int>(p1.x), static_cast<int>(p1.y));
    }

    void DrawTriangleFilled(Vector2f p1, Vector2f p2, Vector2f p3, SDL_Color color) override {
        // Placeholder: Implement filled triangle drawing (e.g., using SDL_gfx or custom algorithm)
        std::cout << "DrawTriangleFilled at points (" << p1.x << ", " << p1.y << "), ("
                  << p2.x << ", " << p2.y << "), (" << p3.x << ", " << p3.y << ")\n";
    }

    void DrawCircleFilled(Vector2f center, float radius, SDL_Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int cx = static_cast<int>(center.x);
        int cy = static_cast<int>(center.y);
        int r = static_cast<int>(radius);
        for (int w = 0; w < r * 2; w++) {
            for (int h = 0; h < r * 2; h++) {
                int dx = r - w; // horizontal offset
                int dy = r - h; // vertical offset
                if ((dx*dx + dy*dy) <= (r * r)) {
                    SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }
private:
    void Shutdown() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        renderer = nullptr;
        window = nullptr;
    }
};