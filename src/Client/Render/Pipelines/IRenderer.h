#pragma once
#include <SDL2/SDL_pixels.h>
#include <string>
#include "Util/GMath.h"

class GameObject;

// Abstract rendering backend
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Initialize() = 0;
    virtual void ClearFrame() = 0;

    virtual void DrawGameObject(const GameObject& obj) = 0;
    virtual void DrawLine(Vector2f from, Vector2f to, SDL_Color color) = 0;
    virtual void DrawCircle(Vector2f center, float radius, SDL_Color color) = 0;
    virtual void DrawCircleFilled(Vector2f center, float radius, SDL_Color color) = 0;

    virtual void DrawTextSmooth(const std::string& text, Vector2f position, int fontSize) = 0;
    virtual void DrawTextFixed(const std::string& text, Vector2f position, int fontSize) = 0;

    virtual void DrawRect(Vector2f topLeft, Vector2f rightDown, SDL_Color color) = 0;
    virtual void DrawTriangle(Vector2f p1, Vector2f p2, Vector2f p3, SDL_Color color) = 0;
    virtual void DrawTriangleFilled(Vector2f p1, Vector2f p2, Vector2f p3, SDL_Color color) = 0;

    virtual void PresentFrame() = 0;

    virtual void SetTitle(const std::string& title) = 0;
};
