#pragma once

#include "../../Util/GMath.h"
#include "AliveEntity.h"

class LogicalPlayer;

class PlayerEntity : public AliveEntity {
private:
    std::weak_ptr<LogicalPlayer> controller;
public:
    PlayerEntity(const std::weak_ptr<LogicalPlayer>& ctrl) 
        : controller(ctrl) {}
    PlayerEntity() = default;
    ~PlayerEntity() override = default;

    void SetPlayer(const std::weak_ptr<LogicalPlayer>& ctrl) {
        controller = ctrl;
    }

    std::weak_ptr<LogicalPlayer> GetPlayer() const {
        return controller;
    }

    void Tick(float dt) override;
};