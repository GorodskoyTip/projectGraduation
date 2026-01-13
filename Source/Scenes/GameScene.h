#pragma once

#include "axmol.h"

class Player;

class GameScene : public ax::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;
    virtual void update(float dt) override;

private:
    Player* player = nullptr;
    Node* world = nullptr;
};
