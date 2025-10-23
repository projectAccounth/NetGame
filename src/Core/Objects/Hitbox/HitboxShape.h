#pragma once

#include "Common/Network/PacketCodec.h"
#include <string>

enum class HitboxShapeType {
    Rectangle,
    Circle,
    Polygon
};

class HitboxShape {
public:
    virtual ~HitboxShape() = default;
    virtual HitboxShapeType GetType() const = 0;
    virtual std::string ToString() const = 0;
    virtual void Encode(PacketCodec& codec) const = 0;
    virtual void Decode(PacketCodec& codec) = 0;
};

class RectShape : public HitboxShape {
    Rect2d bounds;
    float rotation; // degrees or radians

public:
    RectShape(const Rect2d& b, float r = 0.0f) : bounds(b), rotation(r) {}
    RectShape(): bounds({0, 0, 1, 1}), rotation(0) {};

    HitboxShapeType GetType() const override { return HitboxShapeType::Rectangle; }
    const Rect2d& GetBounds() const { return bounds; }
    float GetRotation() const { return rotation; }

    void Encode(PacketCodec& codec) const override {
        codec.WriteRect2(bounds);
        codec.Write<float>(rotation);
    }

    void Decode(PacketCodec& codec) override {
        bounds = codec.ReadRect2();
        rotation = codec.Read<float>();
    }

    std::string ToString() const override {
        return "Rect(bounds=" + bounds.ToString() + ", rotation=" + std::to_string(rotation) + ")";
    }
};

class CircleShape : public HitboxShape {
    Vector2d center;
    float radius;

public:
    CircleShape(const Vector2d& c, float r) : center(c), radius(r) {}
    CircleShape(): center(), radius() {}

    HitboxShapeType GetType() const override { return HitboxShapeType::Circle; }

    void Encode(PacketCodec& codec) const override {
        codec.WriteVector2(center);
        codec.Write<float>(radius);
    }

    Vector2d GetCenter() const { return center; }
    float GetRadius() const { return radius; }

    void Decode(PacketCodec& codec) override {
        center = codec.ReadVector2();
        radius = codec.Read<float>();
    }

    std::string ToString() const override {
        return "Circle(center=" + center.ToString() + ", radius=" + std::to_string(radius) + ")";
    }
};

class PolygonShape : public HitboxShape {
    std::vector<Vector2d> vertices;

public:
    PolygonShape(const std::vector<Vector2d>& verts) : vertices(verts) {}
    PolygonShape(): vertices() {}

    HitboxShapeType GetType() const override { return HitboxShapeType::Polygon; }

    void Encode(PacketCodec& codec) const override {
        codec.Write<uint32_t>(vertices.size());
        for (const auto& v : vertices) codec.WriteVector2(v);
    }

    void Decode(PacketCodec& codec) override {
        uint32_t count = codec.Read<uint32_t>();
        vertices.clear();
        for (uint32_t i = 0; i < count; ++i) {
            vertices.push_back(codec.ReadVector2());
        }
    }

    std::vector<Vector2d> GetVertices() const { return vertices; }

    std::string ToString() const override {
        std::string result = "Polygon(vertices=[";
        for (const auto& v : vertices) result += v.ToString() + ", ";
        if (!vertices.empty()) result.pop_back(), result.pop_back();
        result += "])";
        return result;
    }
};