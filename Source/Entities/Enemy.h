#pragma once
#include "axmol.h"
#include "Entities/EnemyTypes.h"
#include "Core/PhysicsEntity.h"

enum class EnemyState
{
    Idle,
    Move,
    Fall,
    Attack,
    Hit,
    Dead
};

class Player;

class Enemy : public PhysicsEntity
{
public:
    virtual bool initBase();
    virtual void update(float dt) override;

    void setTarget(Player* player) { target = player; }

    ax::Rect getPhysicsRect() const override;

    virtual void updateAI(float dt) = 0;

    void receiveDamage(float amount, int attackID);
    void onDeath();
    bool isDead() const { return state == EnemyState::Dead; }

    void setState(EnemyState newState);
    ax::Animation* createAnimation(const std::string& entity, const std::string& prefix, float delay);
    virtual void updateAnimation() = 0;

    ax::Rect getHurtBox() const;
    bool readyToRemove() const { return state == EnemyState::Dead && deathTimer <= 0.0f; }

    virtual void updateAttack(float dt) = 0;
    ax::Rect getHitBox() { return hitBox; }
    bool isAttackActive() { return attackActive; }
    float getAttackDamage() { return attackDamage; }
    virtual void startAttack() = 0;

protected:
    Player* target = nullptr;

    EnemyState state = EnemyState::Idle;

    EnemyType type;

    ax::Vec2 velocity;
    bool onGround    = false;
    bool facingRight = false;

    float hp;

    bool isAggro = false;
    float aggroRange;
    float deaggroRange;
    float attackRange;

    float hitTimer           = 0.0f;
    int lastReceivedAttackID = -1;

    ax::Rect hitBox;
    bool attackActive       = false;
    float attackTimer       = 0.0f;
    float attackCooldown    = 0.0f;
    float attackDuration    = 0.0f;
    float attackActiveStart = 0.f;
    float attackActiveEnd   = 0.f;
    float attackElapsed     = 0.f;
    int attackDamage;

    virtual void handleIdle(float dt)   = 0;
    virtual void handleMove(float dt)   = 0;
    virtual void handleFall(float dt)   = 0;
    virtual void handleAttack(float dt) = 0;
    virtual void handleHit(float dt)    = 0;
    virtual void handleDeath(float dt)  = 0;

    float deathTimer = 0.0f;
};
