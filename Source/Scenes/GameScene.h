#pragma once

#include "axmol.h"
#include "Core/Collider.h"
#include "Core/PhysicsSystem.h"

#include <vector>

class Player;
class Canine;

class GameScene : public ax::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void updateCamera(float dt);

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    virtual void onEnter() override;
    virtual void onExit() override;

    void drawDebug();

    void updatePlayerAttack(float dt);
    void updateEnemyAttack(float dt);

private:
    PhysicsSystem physics;

    Player* player          = nullptr;
    Canine* canine          = nullptr;
    Node* world             = nullptr;
    bool debugPhysics       = false;
    bool debugHurtBox       = false;
    bool debugHitBox        = false;
    ax::DrawNode* debugDraw = nullptr;
};
