#include "Core/Components/Component.h"
#include "Core/Objects/PlayerEntity.h"
#include "Common/Network/PacketCodec.h"
#include "Core/Components/ComponentRegistry.h"
#include "Core/Objects/Hitbox/HitboxShapeRegistry.h"
#include <cassert>

// Network encode/decode test

int main() {
    PlayerEntity player;
    PlayerEntity decoded;
    PacketCodec codec;

    codec.Reset();

    ComponentRegistry::RegStatic();
    HitboxShapeRegistry::RegStatic();

    player.Move(Vector2d(5, 5));
    player.SetAcceleration(Vector2d(1, 0));
    player.TakeDamage(25);
    player.GetHitbox()->AddHitbox(std::make_unique<RectShape>(Rect2d { 0.0, 0.0, 25.0, 100.0 }, 0.0f));
    player.AddTag("Player");

    for (int i = 0; i < 60; ++i) player.Tick(1.0f / 60.0f);

    auto preDump = player.Dump();
    player.Encode(codec);

    auto packetSize = codec.Size();

    // decode into a clean instance
    decoded.Decode(codec);

    codec.Reset();

    auto postDump = decoded.Dump();

    std::cout << "Before serialization:\n" << preDump << '\n';
    std::cout << "After serialization:\n"  << postDump << '\n';
    std::cout << "Encoded size: " << packetSize << '\n';

    assert(preDump == postDump);

    return 0;
}