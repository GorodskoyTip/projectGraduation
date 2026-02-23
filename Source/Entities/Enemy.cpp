#include "Enemy.h"

USING_NS_AX;

static constexpr float MOVE_SPEED = 80.0f;
static constexpr float GRAVITY    = -900.0f;

bool Enemy::initBase()
{
    velocity = Vec2::ZERO;
    state    = EnemyState::Idle;
    return true;
}

ax::Rect Enemy::getPhysicsRect() const
{
    constexpr float W = 30;
    constexpr float H = 30;

    auto pos = getPosition();
    return Rect(pos.x - W / 2, pos.y - H / 2, W, H);
}

ax::Rect Enemy::getHurtBox() const
{
    constexpr float W = 30;
    constexpr float H = 30;

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

void Enemy::handleDeath(float dt) {}

void Enemy::updateAI(float dt) {}

void Enemy::receiveDamage(float amount)
{
    if (!isInvincible && hp > 0)
    {
        hp                 = std::max(0.0f, hp - amount);
        isInvincible       = true;
        invincibilityTimer = 1;
        AXLOG("Damage received by canine: %f", amount);
    }
}

ax::Animation* Enemy::createAnimation(const std::string& entity, const std::string& prefix, float delay)
{
    Vector<SpriteFrame*> frames;
    int index = 1;

    while (true)
    {
        std::string name   = entity + "_" + prefix + "_" + std::to_string(index) + ".png";
        SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);

        if (frame == nullptr)
            break;

        frames.pushBack(frame);
        index++;
    }

    if (frames.empty())
    {
        AXLOG("No frames found for %s", prefix.c_str());
        return nullptr;
    }

    auto animation = Animation::createWithSpriteFrames(frames, delay);
    animation->retain();
    return animation;
}

void Enemy::updateAnimation() {}

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
        handleDeath(dt);
        break;
    }
    updateAnimation();

    if (invincibilityTimer > 0)
        invincibilityTimer -= dt;
    else if (invincibilityTimer <= 0)
    {
        invincibilityTimer = 0;
        isInvincible       = false;
    }

    velocity.y += GRAVITY * dt;
}
