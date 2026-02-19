#pragma once
#include "axmol.h"

enum class EnemyState
{
    Idle,
    Move,
    Fall,
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
    void setTarget(Player* player);

    void setState(EnemyState newState);
    ax::Animation* createAnimation(const std::string& entity, const std::string& prefix, float delay);
    virtual void updateAnimation();

    void receiveDamage(float amount);
    void onDeath();
    bool isDead() const { return state == EnemyState::Dead; }

protected:
    ax::Vec2 velocity;

    bool facingRight = false;
    bool onGround  = false;

    Player* target = nullptr;

    float aggroRange;
    float attackRange;

    EnemyState state = EnemyState::Idle;

    float hp;

    virtual void handleIdle(float dt);
    virtual void handleMove(float dt);
    virtual void handleFall(float dt);
    virtual void handleDeath(float dt);

};
