#pragma once

#include <vector>
#include "Collider.h"

class Player;
class Enemy;
class Boss;

class PhysicsSystem
{
public:
    void addCollider(const Collider& col);
    bool hasGroundBelow(const ax::Vec2& point) const;
    bool hasWallAhead(const ax::Rect& rect, float dir) const;
    void updatePlayer(Player* player, float dt);
    void updateEnemy(Enemy* enemy, float dt);
    void updateBoss(Boss* boss, float dt);

    const std::vector<Collider>& getColliders() const;

private:
    std::vector<Collider> colliders;

    void playerMoveAndCollideX(Player* player, float dt);
    void playerMoveAndCollideY(Player* player, float dt);
    void enemyMoveAndCollideX(Enemy* enemy, float dt);
    void enemyMoveAndCollideY(Enemy* enemy, float dt);
    void bossMoveAndCollideX(Boss* boss, float dt);
    void bossMoveAndCollideY(Boss* boss, float dt);
};
