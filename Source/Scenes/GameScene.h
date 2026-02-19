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


private:
    Player* player = nullptr;
    Canine* canine = nullptr;
    Node* world    = nullptr;
    PhysicsSystem physics;
    bool debugPhysics = true;
    ax::DrawNode* debugDraw = nullptr;
};
