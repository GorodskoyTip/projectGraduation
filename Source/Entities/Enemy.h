#pragma once
#include "axmol.h"

enum class EnemyState
{
    Idle,
    Patrol,
    Dead
};

class Enemy : public ax::Sprite
{
public:
    static Enemy* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void changeDirection();

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);
    ax::Vec2 getVelocity();
    void setVelocityX(float x);
    void setVelocityY(float y);

    void receiveDamage();
    void onDeath();

private:
    int direction = -1;
    bool onGround = false;
    ax::Vec2 velocity;

    EnemyState state = EnemyState::Idle;

    float hp;

};
