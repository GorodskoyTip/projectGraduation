#pragma once

#include <vector>
#include "Collider.h"

class Player;

class PhysicsSystem
{
public:
    void addCollider(const Collider& col);
    void update(Player* player, float dt);

private:
    std::vector<Collider> colliders;

    void moveAndCollideX(Player* player, float dt);
    void moveAndCollideY(Player* player, float dt);
};
