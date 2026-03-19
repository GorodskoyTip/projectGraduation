#pragma once
#include "Boss.h"

enum class WerewolfAttack
{
    Gnaw,
    Strike,
    Uppercut
};

class Werewolf : public Boss
{
public:
    static Werewolf* create();
    virtual bool init() override;

private:
    WerewolfAttack currentAttack;

    void updateAI(float dt) override;

    void handleIdle(float dt) override;
    void handleMove(float dt) override;
    void handleJump(float dt) override;
    void handleAttack(float dt) override;
    void handleHit(float dt) override;
    void updateAttack(float dt) override;

    void updateAnimation() override;

    void chooseAttack();
    void startAttack();

    ax::Animation* idleAnim;
    ax::Animation* runAnim;
    ax::Animation* jumpAnim;

    ax::Animation* attack1Anim;
    ax::Animation* attack2Anim;
    ax::Animation* attack3Anim;

    ax::Animation* hitAnim;
    ax::Animation* deathAnim;

    BossState currentAnimationState = BossState::Idle;
    WerewolfAttack lastAttack;

    float attackElapsed     = 0.f;
    float attackActiveStart = 0.f;
    float attackActiveEnd   = 0.f;

    float hitTimer = 0.f;

    bool facingRight = true;
};
