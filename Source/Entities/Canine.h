#pragma once
#include "Enemy.h"

class Canine final : public Enemy
{
public:
    static Canine* create();
    virtual bool init() override;

    void updateAI(float dt) override;

private:
    void handleIdle(float dt) override;
    void handleMove(float dt) override;
    void handleFall(float dt) override;
    void handleDead(float dt) override;

    ax::Animation* idleAnim;
    ax::Animation* runAnim;
    ax::Animation* fallAnim;
    ax::Animation* deadAnim;
};
