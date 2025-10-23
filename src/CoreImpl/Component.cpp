#include "Core/Components/Component.h"
#include "Core/Instance.h"
#include <memory>

Component::Component() : owner(), enabled(true) {}
Component::~Component() = default;

void Component::OnAttach(Instance* gameObject) {
    owner = gameObject;
}

void Component::OnDetach() {
    owner = nullptr;
}

void Component::Tick(float /*dt*/) {}

void Component::SetEnabled(bool value) {
    if (enabled == value) return;
    enabled = value;
    if (enabled) Enabled.Fire();
    else Disabled.Fire();
}

bool Component::IsEnabled() const { return enabled; }
Instance* Component::GetOwner() const { return owner; }