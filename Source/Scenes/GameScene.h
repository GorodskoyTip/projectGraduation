#pragma once

#include "axmol.h"
#include "Core/Collider.h"
#include "Core/PhysicsSystem.h"

#include <vector>

class Player;

class GameScene : public ax::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void updateCamera(float dt);

private:
    Player* player = nullptr;
    Node* world    = nullptr;
    PhysicsSystem physics;
};
