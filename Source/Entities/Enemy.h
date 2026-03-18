#pragma once
#include "axmol.h"
#include "Entities/EnemyTypes.h"

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

class Enemy : public ax::Sprite
{
public:
    virtual bool initBase();
    virtual void update(float dt) override;

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);
    ax::Vec2 getVelocity();
    void setVelocityX(float x);
    void setVelocityY(float y);

    virtual void updateAI(float dt);
    void setTarget(Player* player) { target = player; }

    void setState(EnemyState newState);
    ax::Animation* createAnimation(const std::string& entity, const std::string& prefix, float delay);
    virtual void updateAnimation();

    ax::Rect getHurtBox() const;
    void receiveDamage(float amount, int attackID);
    void onDeath();
    bool isDead() const { return state == EnemyState::Dead; }
    bool readyToRemove() const { return state == EnemyState::Dead && deathTimer <= 0.0f; }

    virtual void updateAttack(float dt);
    ax::Rect getHitBox() { return hitBox; }
    bool isAttackActive() { return attackActive; }
    float getAttackDamage() { return attackDamage; }
    virtual void startAttack();

protected:
    EnemyType type;

    ax::Vec2 velocity;

    bool facingRight = false;
    bool onGround  = false;

    Player* target = nullptr;

    bool isAggro = false;
    float aggroRange;
    float deaggroRange;
    float attackRange;

    EnemyState state = EnemyState::Idle;

    float hp;

    virtual void handleIdle(float dt);
    virtual void handleMove(float dt);
    virtual void handleFall(float dt);
    virtual void handleAttack(float dt);
    virtual void handleHit(float dt);
    virtual void handleDeath(float dt);

    float hitTimer           = 0.0f;
    int lastReceivedAttackID = -1;
    float deathTimer         = 0.0f;

    ax::Rect hitBox;
    bool attackActive = false;
    float attackTimer = 0.0f;
    float attackCooldown = 0.0f;
    float attackDuration = 0.0f;

    float attackActiveStart = 0.f;
    float attackActiveEnd   = 0.f;
    float attackElapsed     = 0.f;

    int attackDamage;
};
