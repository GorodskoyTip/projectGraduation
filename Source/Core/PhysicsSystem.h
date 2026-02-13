#pragma once

#include <vector>
#include "Collider.h"

class Player;
class Enemy;

class PhysicsSystem
{
public:
    void addCollider(const Collider& col);
    void updatePlayer(Player* player, float dt);
    void updateEnemy(Enemy* enemy, float dt);
    bool hasGroundBelow(const ax::Vec2& point) const;

private:
    std::vector<Collider> colliders;

    void moveAndCollideX(Player* player, float dt);
    void moveAndCollideY(Player* player, float dt);
};
