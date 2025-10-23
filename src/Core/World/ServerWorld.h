#pragma once

#include "CollisionMatrix.h"
#include "Common/Packets/S2C/ReplicationPacket.h"
#include "Core/Objects/CollisionGroups.h"
#include "RaycastHit.h"
#include "Util/GMath.h"
#include "Core/Objects/Entity.h"
#include "Core/Player/Player.h"
#include <optional>
#include <vector>
#include "IWorld.h"
#include "World.h"
#include "Server/NetworkSystem.h"

class ServerWorld : public World {
private:
public:
    ServerWorld(): World(true) {};

    void QueueReplication(GameObject* obj) {
        if (!isServer) return;
        if (std::find(replicationQueue.begin(), replicationQueue.end(), obj) == replicationQueue.end())
            replicationQueue.push_back(obj);
    }

    void Tick(float dt) override {
        World::Tick(dt);
    }

    void ProcessReplicationQueue(NetworkSystem& network) {
        if (!isServer) return;

        ReplicationPacket packet;

        for (GameObject* obj : replicationQueue) {
            if (!obj || !obj->IsDirty())
                continue;

            packet.AddObject(obj, ReplicationPacket::ReplicationType::FullSync);
            obj->ClearDirty();
        }

        network.broadcast(packet);

        replicationQueue.clear();
    }

    ~ServerWorld() override = default;
};