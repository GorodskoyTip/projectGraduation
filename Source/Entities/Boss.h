#pragma once
#include "axmol.h"
#include "Core/PhysicsEntity.h"

class Player;

enum class BossState
{
    Idle,
    Move,
    Jump,
    Attack,
    Hit,
    Dead
};

class Boss : public PhysicsEntity
{
public:
    virtual bool initBase();
    virtual void update(float dt) override;

    void setTarget(Player* p) { target = p; }

    ax::Rect getPhysicsRect() const override;

    void setArena(const ax::Rect& rect);
    bool isFightStarted() const { return fightStarted; }

    virtual void receiveDamage(float amount);
    bool isDead() const { return state == BossState::Dead; }

    float getAttackDamage() { return attackDamage; }
    bool isAttackActive() const { return attackActive; }
    ax::Rect getHitBox() const { return hitBox; }

protected:
    Player* target = nullptr;

    BossState state = BossState::Idle;

    float hp = 300;

    ax::Rect arena;
    bool fightStarted = false;

    bool attackActive    = false;
    float attackTimer    = 0;
    float attackCooldown = 0;
    float attackDamage   = 0;

    ax::Rect hitBox;

    ax::Animation* createAnimation(const std::string& entity, const std::string& prefix, float delay);

    virtual void updateAI(float dt)     = 0;
    virtual void handleIdle(float dt)   = 0;
    virtual void handleMove(float dt)   = 0;
    virtual void handleJump(float dt)   = 0;
    virtual void handleAttack(float dt) = 0;
    virtual void handleHit(float dt)    = 0;
    virtual void updateAttack(float dt) = 0;
    virtual void updateAnimation()      = 0;

private:
    float deathTimer = 0;
};
