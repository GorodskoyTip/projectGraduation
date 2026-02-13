#pragma once

#include "axmol.h"
#include "Core/Collider.h"
#include "Core/PhysicsSystem.h"

#include <vector>

class Player;
class Enemy;

class GameScene : public ax::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void updateCamera(float dt);

    void spawnEnemy(const ax::Vec2& position);


private:
    Player* player = nullptr;
    std::vector<Enemy*> enemies;
    Node* world    = nullptr;
    PhysicsSystem physics;
};
