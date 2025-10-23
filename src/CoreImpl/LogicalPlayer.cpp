#include "Core/Player/Player.h"
#include "Core/World/World.h"

int LogicalPlayer::GetPlayerID() const { return playerID; }
const std::string& LogicalPlayer::GetName() const { return name; }
void LogicalPlayer::SetName(const std::string& n) { name = n; }

void LogicalPlayer::SetPlayerEntity(PlayerEntity* ent) {
        if (controlledEntity == ent) return;
        if (controlledEntity) {
        }

        controlledEntity = ent;
        if (controlledEntity) {
            controlledEntity->Destroyed.Connect([this]() {
                controlledEntity = nullptr;
                OnEntityUnassigned.Fire();
            });
        }
    }

PlayerEntity* LogicalPlayer::GetPlayerEntity() const {
    return controlledEntity;
}

void LogicalPlayer::Encode(PacketCodec& codec) const {
    codec.Write(playerID);
    codec.WriteString(name);
    if (controlledEntity)
        codec.WriteUUID(controlledEntity->GetUUID());
    else
        codec.WriteUUID(Util::UUID::null());
}

void LogicalPlayer::Decode(PacketCodec& codec) {
    playerID = codec.Read<int>();
    name = codec.ReadString();
    UUID entityID = codec.ReadUUID();
    if (entityID != UUID::null()) {
        auto entity = dynamic_cast<PlayerEntity*>(GetWorld()->Find(entityID));
        controlledEntity = entity;
    }
}