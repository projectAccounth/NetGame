#include "Core/World/World.h"
#include "Core/Objects/Entity.h"
#include "Core/World/CollisionMatrix.h"
#include "Util/Physics/RectSwept.h"

void World::Tick(float dt) {
    // integrate velocities
    for (auto& obj : objects) {
        auto phys = obj->GetPhysicalProperties();
        if (phys && !phys->IsAnchored()) {
            Vector2d v = obj->GetVelocity();
            Vector2d a = obj->GetAcceleration();
            obj->SetVelocity(v + a * dt);
        }
        obj->Tick(dt);
    }

    double remaining = dt;
    const double EPS = 1e-6;
    const double MIN_STEP = 1e-5; // at least this many seconds when a collision is detected to make progress

    while (remaining > EPS) {
        double earliest = remaining;
        GameObject* hitObjA = nullptr;
        GameObject* hitObjB = nullptr;
        const Hitbox* hitboxA = nullptr;
        const Hitbox* hitboxB = nullptr;
        Vector2d hitNormal{0,0};
        bool hitIsTrigger = false;

        // find earliest collision in the remaining interval
        for (size_t i = 0; i < objects.size(); ++i) {
            for (size_t j = i + 1; j < objects.size(); ++j) {
                auto* a = objects[i].get();
                auto* b = objects[j].get();

                auto ha = a->GetComponent<HitboxComponent>();
                auto hb = b->GetComponent<HitboxComponent>();
                if (!ha || !hb) continue;

                auto ta = a->GetComponent<TransformComponent>();
                auto tb = b->GetComponent<TransformComponent>();
                if (!ta || !tb) continue;

                auto pa = a->GetPhysicalProperties();
                auto pb = b->GetPhysicalProperties();
                if ((pa && pa->IsAnchored()) && (pb && pb->IsAnchored()))
                    continue;

                Vector2d va = a->GetVelocity();
                Vector2d vb = b->GetVelocity();

                for (const auto& hbA : ha->GetHitboxes()) {
                    for (const auto& hbB : hb->GetHitboxes()) {
                        if (CollisionMatrix::ShouldCollide(hbA.group, hbB.group))
                            continue;

                        Vector2d dispA = va * remaining;
                        Vector2d dispB = vb * remaining;
                        Vector2d posA = ta->GetPosition();
                        Vector2d posB = tb->GetPosition();

                        SweepResult res = SweptShapeCollision(
                            *hbA.shape, posA, dispA,
                            *hbB.shape, posB, dispB,
                            remaining
                        );

                        if (!res.hit) continue;

                        // clamp TOI to [0,1] to avoid floating error causing toi slightly outside
                        res.toi = std::max(0.0, std::min(1.0, res.toi));
                        res.isTrigger = (hbA.isTrigger || hbB.isTrigger);

                        double impactTimeAbsolute = res.toi * remaining;

                        // prefer strictly earlier impacts
                        if (impactTimeAbsolute < earliest) {
                            earliest = impactTimeAbsolute;
                            hitObjA = a;
                            hitObjB = b;
                            hitboxA = &hbA;
                            hitboxB = &hbB;
                            hitNormal = res.normal;
                            hitIsTrigger = res.isTrigger;
                        }
                    }
                }
            }
        }

        // If we found a collision at 'earliest' (could be 0)
        if (hitObjA && hitObjB && hitboxA && hitboxB) {
            // If earliest is extremely small (including 0), advance by a tiny step to avoid stalling.
            double step = earliest;
            if (step < MIN_STEP) step = std::min(MIN_STEP, remaining);

            // Move all non-anchored objects forward by 'step'
            for (auto& obj : objects) {
                if (auto phys = obj->GetPhysicalProperties(); !phys || !phys->IsAnchored()) {
                    obj->Move(obj->GetVelocity() * step);
                }
            }

            remaining -= step;

            if (hitObjA) hitObjA->OnCollision(hitObjB);
            if (hitObjB) hitObjB->OnCollision(hitObjA);

            // If this collision is a trigger, call trigger handlers but do NOT do physical resolve.
            if (hitIsTrigger) {
                // continue to next iteration (we already advanced by 'step' so will make progress)
                // continue;
            }

            // Non-trigger: resolve physically.
            // NOTE: ResolveCollision should expect the time step you just advanced (step),
            // or you can pass the impact time (earliest) depending on ResolveCollision semantics.
            ResolveCollision(hitObjA, hitObjB, hitNormal, static_cast<float>(step));

            // after resolving, continue the loop to find further collisions within remaining time
            continue;
        }

        // No collision detected in 'remaining' interval -> move full remaining and finish
        for (auto& obj : objects) {
            if (auto phys = obj->GetPhysicalProperties(); !phys || !phys->IsAnchored()) {
                obj->Move(obj->GetVelocity() * remaining);
            }
        }

        remaining = 0.0;
        break;
    } // end while
}
void World::ResolveCollision(GameObject* a, GameObject* b, const Vector2d& n, float dt) {
    auto pa = a->GetPhysicalProperties();
    auto pb = b->GetPhysicalProperties();

    bool anchoredA = pa && pa->IsAnchored();
    bool anchoredB = pb && pb->IsAnchored();

    float ma = (pa && !anchoredA) ? pa->GetMass() : 0.0f;
    float mb = (pb && !anchoredB) ? pb->GetMass() : 0.0f;
    float invMa = (ma > 0.0f) ? 1.0f / ma : 0.0f;
    float invMb = (mb > 0.0f) ? 1.0f / mb : 0.0f;

    if (invMa == 0.0f && invMb == 0.0f)
        return; // both static

    Vector2d va = a->GetVelocity();
    Vector2d vb = b->GetVelocity();
    Vector2d relV = va - vb;

    float relN = relV.Dot(n);
    if (relN >= 0.0f)
        return; // moving apart

    const float restitution = 0.0f; // no bounce for top-down
    float invMassSum = invMa + invMb;
    if (invMassSum <= 0.0f)
        return;

    // --- NORMAL IMPULSE (cancel penetration velocity) ---
    float j = (-(1.0f + restitution) * relN) / invMassSum;
    Vector2d impulse = j * n;

    if (!anchoredA)
        a->SetVelocity(va + impulse * invMa);
    if (!anchoredB)
        b->SetVelocity(vb - impulse * invMb);

    // --- DAMP NORMAL ACCELERATION (stop continuous pushing) ---
    if (!anchoredA && pa) {
        Vector2d accA = a->GetAcceleration();
        float accN = accA.Dot(n);
        if (accN < 0.0f) // pushing into collision
            accA -= accN * n; // remove normal component
        a->SetAcceleration(accA);
    }

    if (!anchoredB && pb) {
        Vector2d accB = b->GetAcceleration();
        float accN = accB.Dot(-n); // opposite normal for B
        if (accN < 0.0f)
            accB -= accN * (-n);
        b->SetAcceleration(accB);
    }

    // --- SEPARATION BIAS (prevent re-collision jitter) ---
    const float separationBias = 0.001f;
    if (!anchoredA)
        a->Move(n * (separationBias * invMa / invMassSum));
    if (!anchoredB)
        b->Move(-n * (separationBias * invMb / invMassSum));

    // --- OPTIONAL: later 3D physics or rolling friction ---
    // You could add tangential friction impulses here if you ever
    // extend to 3D dynamics. For now, keep this commented out.
}


void World::ProcessDestroyQueue() {
    if (destroyQueue.empty()) return;

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [this](const std::unique_ptr<GameObject>& obj) {
            return obj->ShouldDestroy() &&
                   std::find(destroyQueue.begin(), destroyQueue.end(), obj.get()) != destroyQueue.end();
        }),
        objects.end());

    destroyQueue.clear();
}

void World::RemoveObject(const GameObject* obj) {
    if (!obj) return;

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [obj](const std::unique_ptr<GameObject>& ptr) {
            if (ptr.get() == obj) {
                ptr->SetWorld(nullptr);
                return true;
            }
            return false;
        }), objects.end());

    destroyQueue.erase(std::remove(destroyQueue.begin(), destroyQueue.end(), obj), destroyQueue.end());
}

void World::RemoveObject(const UUID& uuid) {
    GameObject* obj = Find(uuid);
    if (!obj) return;

    RemoveObject(obj);
}

UUID World::AddObject(std::unique_ptr<GameObject> obj) {
    if (!obj) throw std::invalid_argument("Cannot add null GameObject");

    GameObject* rawPtr = obj.get();
    rawPtr->SetWorld(this);
    UUID id = rawPtr->GetUUID();
    objects.push_back(std::move(obj));

    rawPtr->Destroyed.Connect([this, rawPtr]() {
        if (std::find(destroyQueue.begin(), destroyQueue.end(), rawPtr) == destroyQueue.end()) {
            destroyQueue.push_back(rawPtr);
        }
    });

    return id;
}

std::string World::Dump() const {
    std::string result = "World(\n";
    for (auto& object : GetObjects()) {
        result += object->Dump() + '\n';
    }
    result += ")";
    return result;
}

World::~World() = default;