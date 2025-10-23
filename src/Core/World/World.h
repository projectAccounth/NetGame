#pragma once

#include "CollisionMatrix.h"
#include "Core/Objects/CollisionGroups.h"
#include "Core/Objects/PlayerEntity.h"
#include "RaycastHit.h"
#include "Util/GMath.h"
#include "Core/Objects/Entity.h"
#include "Core/Player/Player.h"
#include <optional>
#include <vector>
#include "IWorld.h"

class World : public IWorld {
protected:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<GameObject*> destroyQueue;
    std::vector<GameObject*> replicationQueue;
    std::vector<std::unique_ptr<LogicalPlayer>> players;

    bool isServer;

public:
    World(bool isServer): isServer(isServer) {};

    bool IsServer() override { return isServer; }

    void Tick(float dt) override;

    template <typename T, typename... Args>
    T& SpawnObject(Args&&... args) {
        static_assert(std::is_base_of_v<GameObject, T>, "T must derive from GameObject");

        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *obj;
        ref.SetWorld(this);

        // When destroyed, mark for removal
        ref.Destroyed.Connect([this, &ref]() {
            destroyQueue.push_back(&ref);
        });

        objects.push_back(std::move(obj));
        return ref;
    }

    PlayerEntity& SpawnPlayer(std::unique_ptr<LogicalPlayer> player, Vector2d position = Vector2d()) {
        PlayerEntity& entity = SpawnObject<PlayerEntity>();
        player->SetPlayerEntity(&entity);
        
        entity.SetPosition(position);
        entity.GetTransform()->SetRotation(0);

        if (std::find(players.begin(), players.end(), player) == players.end())
            AddPlayer(player);

        return entity;
    }

    void AddPlayer(std::unique_ptr<LogicalPlayer>& player) {
        players.push_back(std::move(player));
    }

    void ProcessDestroyQueue();

    void RemoveObject(const GameObject* obj) override;
    UUID AddObject(std::unique_ptr<GameObject> obj) override;

    void RemoveObject(const UUID& id);

    void SpawnPlayer() override {
        // Placeholder implementation
        std::cout << "[World] SpawnPlayer called.\n";
    }

    const GameObject ResolveObject(UUID id) {
        if (!Find(id)) throw std::runtime_error("[World] could not find entity with id " + id.to_string());
        return *Find(id);
    }

    GameObject* Find(UUID id) {
        for (const auto& obj : objects) {
            if (obj->GetUUID() == id)
                return obj.get();
        }
        return nullptr;
    }
    /*
    std::optional<RaycastHit> Raycast(
        const Vector2d& origin,
        const Vector2d& direction,
        double maxDistance,
        CollisionGroup mask = CollisionGroup::DefaultCollidable
    ) {
        Vector2d dir = direction.Normalized();
        std::optional<RaycastHit> nearestHit;
        double nearestDist = maxDistance;

        for (auto& objPtr : objects) {
            GameObject* obj = objPtr.get();
            if (!obj->HasComponent<TransformComponent>() || !obj->HasComponent<HitboxComponent>())
                continue;

            auto transform = obj->GetComponent<TransformComponent>();
            auto hitbox = obj->GetComponent<HitboxComponent>();

            if (hitbox->GetGroup() == CollisionGroup::DefaultNonCollidable)
                continue;

            if (!CollisionMatrix::ShouldCollide(mask, hitbox->GetGroup()))
                continue;

            Rect2 worldRect = hitbox->GetLocalBounds().Translated(transform->GetPosition());

            double tNear;
            if (worldRect.RayIntersectsRect(origin, dir, tNear)) {
                if (tNear >= 0.0 && tNear < nearestDist) {
                    nearestDist = tNear;
                    nearestHit = RaycastHit{
                        .object = obj,
                        .point = origin + dir * tNear,
                        .distance = tNear
                    };
                }
            }
        }

        return nearestHit;
    }
    */

    void ResolveCollision(GameObject* a, GameObject* b, const Vector2d& n, float dt);

    const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return objects; }

    std::string Dump() const;

    ~World() override;
};