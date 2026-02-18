#pragma once
#include "axmol.h"

enum class EnemyState
{
    Idle,
    Move,
    Fall,
    Dead
};

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

    void receiveDamage(float amount);
    void onDeath();

private:
    ax::Vec2 velocity;

    bool moveLeft = false;
    bool moveRight = false;
    bool onGround  = false;

    virtual void handleIdle(float dt);
    virtual void handleMove(float dt);
    virtual void handleFall(float dt);
    virtual void handleDead(float dt);

    EnemyState state = EnemyState::Idle;

    float hp;

};
