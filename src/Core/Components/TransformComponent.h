#pragma once
#include "Component.h"
#include "Util/GMath.h" // define simple float-based 2D vector

class TransformComponent : public Component {
private:
    Vector2d position;
    Vector2d scale;
    Vector2d velocity;
    Vector2d acceleration;
    float rotation; // in degrees

public:
    TransformComponent();
    TransformComponent(const Vector2d& pos, const Vector2d& scl = {1.0f, 1.0f}, float rot = 0.0f);

    // Position
    void SetPosition(const Vector2d& pos);
    const Vector2d& GetPosition() const;
    void Translate(const Vector2d& delta);

    // Rotation
    void SetRotation(float degrees);
    float GetRotation() const;
    void Rotate(float deltaDegrees);

    void SetVelocity(const Vector2d& vel);
    const Vector2d& GetVelocity() const;
    void Move(const Vector2d& delta);
    
    void Accelerate(const Vector2d& accel);
    const Vector2d& GetAcceleration() const;
    void SetAcceleration(const Vector2d& accel);

    // Scale
    void SetScale(const Vector2d& s);
    const Vector2d& GetScale() const;
    void Scale(const Vector2d& factor);

    void Encode(PacketCodec& codec) const override {
        codec.WriteVector2(position);
        codec.WriteVector2(scale);
        codec.WriteVector2(velocity);
        codec.WriteVector2(acceleration);
        codec.WriteFloat(rotation);
    }

    void Decode(PacketCodec& codec) override {
        position = codec.ReadVector2();
        scale = codec.ReadVector2();
        velocity = codec.ReadVector2();
        acceleration = codec.ReadVector2();
        rotation = codec.Read<float>();
    }

    // Helpers
    void ResetTransform();

    std::string Dump() const override {
        return "TransformComponent(position=" + position.ToString() +
               ", scale=" + scale.ToString() +
               ", velocity=" + velocity.ToString() +
               ", acceleration=" + acceleration.ToString() +
               ", rotation=" + std::to_string(rotation) + ")";
    }
};