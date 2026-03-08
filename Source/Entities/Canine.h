#pragma once
#include "Enemy.h"

class Canine final : public Enemy
{
public:
    static Canine* create();
    virtual bool init() override;

    void updateAI(float dt) override;

    void updateAnimation() override;

    void startAttack() override;
    void updateAttack(float dt) override;

private:
    void handleIdle(float dt) override;
    void handleMove(float dt) override;
    void handleFall(float dt) override;
    void handleAttack(float dt) override;
    void handleHit(float dt) override;
    void handleDeath(float dt) override;

    ax::Animation* idleAnim;
    ax::Animation* runAnim;
    ax::Animation* fallAnim;
    ax::Animation* attackAnim;
    ax::Animation* hitAnim;
    ax::Animation* deathAnim;
    EnemyState currentAnimationState = EnemyState::Idle;
};
