#include "../Core/Instance.h"
#include "../Core/Components/ComponentRegistry.h"

Instance::~Instance() {
    if (!destroyed) {
        destroyed = true;
        Destroyed.Fire();
    }

    for (auto& [type, comp] : components)
        comp->OnDetach();

    components.clear();
}

void Instance::SetWorld(World* w) { world = w; }
World* Instance::GetWorld() const { return world; }

void Instance::Tick(float dt) {
    if (destroyed) return;
    for (auto& [type, comp] : components) {
        if (comp->IsEnabled())
            comp->Tick(dt);
    }
}

void Instance::Destroy() {
    if (destroyed) return;
    destroyed = true;
    for (auto& [type, comp] : components)
        comp->OnDetach();
    Destroyed.Fire();
    components.clear();
}

void Instance::Encode(PacketCodec& codec) const {
    codec.WriteUUID(uuid);
    codec.WriteStringArray(tags);

    // Write component count
    codec.Write<uint32_t>((uint32_t)components.size());

    for (const auto& [type, comp] : components) {
        // Each component type should have a registered string or ID
        std::string typeName = ComponentRegistry::GetName(type);
        codec.WriteString(typeName);

        comp->Encode(codec);
    }
}

void Instance::Decode(PacketCodec& codec) {
    uuid = codec.ReadUUID();
    tags = codec.ReadStringArray();

    uint32_t compCount = codec.Read<uint32_t>();

    for (uint32_t i = 0; i < compCount; ++i) {
        std::string typeName = codec.ReadString();

        auto comp = ComponentRegistry::Create(typeName);
        if (!comp) {
            std::cerr << "[Instance] Unknown component type: " << typeName << "\n";
            continue;
        }

        auto& ref = *comp;
        comp->OnAttach(this);
        comp->Decode(codec);
        components[std::type_index(typeid(ref))] = std::move(comp);
    }
}

std::string Instance::Dump() const {
    std::string current = "Instance(UUID=" + uuid.to_string() + ", Tags=[" +
            std::accumulate(tags.begin(), tags.end(), std::string(""),
                            [](const std::string& a, const Tag& b) {
                                return a + (a.empty() ? "" : ", ") + b;
                            }) + "])\n";
    for (auto& [id, comp] : components) {
        current += comp->Dump() + '\n';
    }
    return current; 
}