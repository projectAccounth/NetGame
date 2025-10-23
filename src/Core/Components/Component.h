#pragma once
#include <typeindex>
#include <memory>
#include <unordered_map>
#include "Core/Connection.h"
#include "Common/Network/PacketCodec.h"

class Instance;

class Component {
protected:
    Instance* owner;
    bool enabled;

public:
    Signal<> Enabled;
    Signal<> Disabled;

    Component();
    virtual ~Component();

    virtual void OnAttach(Instance* gameObject);
    virtual void OnDetach();
    virtual void Tick(float dt);

    void SetEnabled(bool value);
    bool IsEnabled() const;

    virtual void Encode(PacketCodec& codec) const = 0;
    virtual void Decode(PacketCodec& codec) = 0;

    Instance* GetOwner() const;

    virtual std::string Dump() const { return ""; }
};
