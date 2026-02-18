#include "Enemy.h"

USING_NS_AX;

static constexpr float MOVE_SPEED = 80.0f;
static constexpr float GRAVITY    = -900.0f;

Enemy* Enemy::create()
{
    auto e = new (std::nothrow) Enemy();
    if (e && e->init())
    {
        e->autorelease();
        return e;
    }
    AX_SAFE_DELETE(e);
    return nullptr;
}

bool Enemy::init()
{
    if (!Sprite::initWithFile("Content/Sprites/CanineBlack/spriteSheet.png"))
        return false;

    velocity = Vec2::ZERO;
    return true;
}

ax::Rect Enemy::getPhysicsRect() const
{
    constexpr float W = 40;
    constexpr float H = 80;

    auto pos = getPosition();
    return Rect(pos.x - W / 2, pos.y - H / 2, W, H);
}

void Enemy::setOnGround(bool value)
{
    onGround = value;
}

void Enemy::changeDirection()
{
    direction *= -1;
}

ax::Vec2 Enemy::getVelocity()
{
    return velocity;
}

void Enemy::setVelocityX(float x)
{
    velocity.x = x;
}

void Enemy::setVelocityY(float y)
{
    velocity.y = y;
}



void Enemy::update(float dt)
{
    switch (state)
    {
    case EnemyState::Idle:
        break;
    case EnemyState::Patrol:
        break;
    case EnemyState::Dead:
        break;
    }
    velocity.x = direction * MOVE_SPEED;
    if (!onGround)
        velocity.y += GRAVITY * dt;
}
