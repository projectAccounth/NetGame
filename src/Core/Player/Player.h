#pragma once

#include <string>
#include <memory>
#include "../Objects/PlayerEntity.h"

class LogicalPlayer : public Instance {
private:
    int playerID;
    std::string name;
    PlayerEntity* controlledEntity;

public:
    Signal<> OnEntityUnassigned;
    explicit LogicalPlayer(int id, const std::string& name)
        : playerID(id), name(name) {}

    int GetPlayerID() const;
    const std::string& GetName() const;
    void SetName(const std::string& n);

    void SetPlayerEntity(PlayerEntity* entity);

    PlayerEntity* GetPlayerEntity() const;

    void Encode(PacketCodec& codec) const override;

    void Decode(PacketCodec& codec) override;

    ~LogicalPlayer() override = default;
};