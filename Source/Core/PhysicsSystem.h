#pragma once

#include <vector>
#include "Collider.h"

class Player;
class Enemy;

class PhysicsSystem
{
public:
    void addCollider(const Collider& col);
    bool hasGroundBelow(const ax::Vec2& point) const;
    void updatePlayer(Player* player, float dt);
    void updateEnemy(Enemy* enemy, float dt);

private:
    std::vector<Collider> colliders;

    void playerMoveAndCollideX(Player* player, float dt);
    void playerMoveAndCollideY(Player* player, float dt);
    void enemyMoveAndCollideX(Enemy* enemy, float dt);
    void enemyMoveAndCollideY(Enemy* enemy, float dt);
};
