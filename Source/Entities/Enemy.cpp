#include "Enemy.h"

USING_NS_AX;

bool Enemy::initBase()
{
    velocity = Vec2::ZERO;
    state    = EnemyState::Idle;
    return true;
}

ax::Rect Enemy::getPhysicsRect() const
{
    constexpr float W = 40;
    constexpr float H = 80;

    auto pos = getPosition();
    return Rect(pos.x - W / 2, pos.y - H / 2, W, H);
}

ax::Vec2 Enemy::getVelocity() {return velocity;}

void Enemy::setOnGround(bool value) {onGround = value;}

void Enemy::setVelocityX(float x) {velocity.x = x;}

void Enemy::setVelocityY(float y) {velocity.y = y;}

void Enemy::handleIdle(float dt) {}

void Enemy::handleMove(float dt) {}

void Enemy::handleFall(float dt) {}

void Enemy::handleDead(float dt) {}

void Enemy::updateAI(float dt) {}

void Enemy::update(float dt)
{
    updateAI(dt);

    switch (state)
    {
    case EnemyState::Idle:
        handleIdle(dt);
        break;
    case EnemyState::Move:
        handleMove(dt);
        break;
    case EnemyState::Fall:
        handleFall(dt);
        break;
    case EnemyState::Dead:
        handleDead(dt);
        break;
    }

    velocity.y += GRAVITY * dt;
}
