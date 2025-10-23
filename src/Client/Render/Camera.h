#pragma once

#include "Core/Objects/GameObject.h"
#include "Util/GMath.h"
class Camera {
private:
    Vector2d position;

    int width = 0, height = 0;
public:
    Camera() = default;
    Camera(int w, int h): width(w), height(h) {}
    ~Camera() = default;

    void SetPosition(const Vector2d& pos) { position = pos; }
    Vector2d GetPosition() const { return position; }

    Vector2i GetDimensions() const { return { width, height }; }
    void SetDimensions(Vector2i dim) { width = dim.x; height = dim.y; }

    int GetWidth() const { return width; }
    void SetWidth(int w) { width = w; }

    int GetHeight() const { return height; }
    void SetHeight(int h) { height = h; }

    bool OutsideCamera(const Vector2d& point) const {
        return point.x < position.x || point.x > position.x + width ||
               point.y < position.y || point.y > position.y + height;
    }

    bool OutsideCamera(const GameObject& object) const {
        auto* transform = object.GetComponent<TransformComponent>();
        if (!transform) return true;
        Vector2d objPos = transform->GetPosition();

        auto hitbox = object.GetHitbox();
        if (!hitbox) return true;

        auto& hitboxes = hitbox->GetHitboxes();
        for (auto& h: hitboxes) {
            if (auto rectShape = dynamic_cast<RectShape*>(h.shape.get())) {
                auto bounds = rectShape->GetBounds().Translated(objPos);
                if (bounds.x + bounds.width < position.x) continue; // left
                if (bounds.x > position.x + width) continue; // right
                if (bounds.y + bounds.height < position.y) continue; // above
                if (bounds.y > position.y + height) continue; // below
                return false; // inside camera
            }
            else if (auto circleShape = dynamic_cast<CircleShape*>(h.shape.get())) {
                auto center = circleShape->GetCenter() + objPos;
                float radius = circleShape->GetRadius();
                if (center.x + radius < position.x) continue; // left
                if (center.x - radius > position.x + width) continue; // right
                if (center.y + radius < position.y) continue; // above
                if (center.y - radius > position.y + height) continue; // below
                return false; // inside camera
            }
            else if (auto polygonShape = dynamic_cast<PolygonShape*>(h.shape.get())) {
                for (const auto& vertex : polygonShape->GetVertices()) {
                    Vector2d worldVertex = vertex + objPos;
                    if (worldVertex.x >= position.x && worldVertex.x <= position.x + width &&
                        worldVertex.y >= position.y && worldVertex.y <= position.y + height) {
                        return false; // inside camera
                    }
                }
            }
        }

        return false;
    }

    bool InsideCamera(const GameObject& object) const {
        return !OutsideCamera(object);
    }

    bool InsideCamera(const Vector2d& point) const {
        return !OutsideCamera(point);
    }
};