#include "Core/Components/TransformComponent.h"
#include "Util/GMath.h"

TransformComponent::TransformComponent()
    : position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0f) {}

TransformComponent::TransformComponent(const Vector2d& pos, const Vector2d& scl, float rot)
    : position(pos), scale(scl), rotation(rot) {}

void TransformComponent::SetPosition(const Vector2d& pos) {
    position = pos;
}

const Vector2d& TransformComponent::GetPosition() const {
    return position;
}

void TransformComponent::Translate(const Vector2d& delta) {
    position += delta;
}

void TransformComponent::SetRotation(float degrees) {
    rotation = degrees;
}

float TransformComponent::GetRotation() const {
    return rotation;
}

void TransformComponent::Rotate(float deltaDegrees) {
    rotation += deltaDegrees;
}

void TransformComponent::SetScale(const Vector2d& s) {
    scale = s;
}

const Vector2d& TransformComponent::GetScale() const {
    return scale;
}

void TransformComponent::Scale(const Vector2d& factor) {
    scale.x *= factor.x;
    scale.y *= factor.y;
}

void TransformComponent::ResetTransform() {
    position = {0.0f, 0.0f};
    scale = {1.0f, 1.0f};
    rotation = 0.0f;
    velocity = {0.0f, 0.0f};
    acceleration = {0.0f, 0.0f};
}

void TransformComponent::SetVelocity(const Vector2d& vel) {
    velocity = vel;
}

const Vector2d& TransformComponent::GetVelocity() const {
    return velocity;
}

void TransformComponent::Move(const Vector2d& delta) {
    position += delta;
}

void TransformComponent::Accelerate(const Vector2d& accel) {
    acceleration = accel;
}

const Vector2d& TransformComponent::GetAcceleration() const {
    return acceleration;
}

void TransformComponent::SetAcceleration(const Vector2d& accel) {
    acceleration = accel;
}