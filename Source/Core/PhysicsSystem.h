#pragma once
#include <vector>
#include "Collider.h"
#include "PhysicsEntity.h"

class PhysicsSystem
{
public:
    void addCollider(const Collider& col);
    bool hasGroundBelow(const ax::Vec2& point) const;
    bool hasWallAhead(const ax::Rect& rect, float dir) const;
    void updatePhysics(PhysicsEntity* body, float dt);

    const std::vector<Collider>& getColliders() const;

private:
    std::vector<Collider> colliders;

    void moveAndCollideX(PhysicsEntity* body, float dt);
    void moveAndCollideY(PhysicsEntity* body, float dt);
};
