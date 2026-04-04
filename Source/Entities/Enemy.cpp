#include "Enemy.h"

USING_NS_AX;

static constexpr float GRAVITY    = -900.0f;

bool Enemy::initBase()
{
    setVelocityX(0);
    setVelocityY(0);
    state    = EnemyState::Idle;
    return true;
}

ax::Rect Enemy::getPhysicsRect() const
{
    constexpr float W = 30;
    constexpr float H = 30;

    auto pos = getPosition();
    return Rect(pos.x - W / 2, pos.y - H, W, H);
}

ax::Rect Enemy::getHurtBox() const
{
    constexpr float W = 30;
    constexpr float H = 30;

    auto pos = getPosition();
    return Rect(pos.x - W / 2, pos.y - H, W, H);
}

void Enemy::receiveDamage(float amount, int attackID)
{
    if (lastReceivedAttackID == attackID)
        return;

    lastReceivedAttackID = attackID;

    hp = std::max(0.0f, hp - amount);
    state = EnemyState::Hit;
    hitTimer = 1.0f;
    isAggro  = true;

    stopAllActions();

    AXLOG("Damage received by enemy: %f", amount);

    if (hp <= 0)
        onDeath();
}

void Enemy::onDeath()
{
    state = EnemyState::Dead;

    attackActive = false;
    attackTimer  = 0;
    setVelocityX(0);
    setVelocityY(0);

    deathTimer = 2.0f;

    stopAllActions();
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

void Enemy::update(float dt)
{
    if (state == EnemyState::Dead)
    {
        handleDeath(dt);
        updateAnimation();
        return;
    }
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
    case EnemyState::Attack:
        handleAttack(dt);
        break;
    case EnemyState::Hit:
        handleHit(dt);
        break;
    case EnemyState::Dead:
        handleDeath(dt);
        break;
    }
    updateAnimation();
    updateAttack(dt);

    setVelocityY(getVelocity().y - GRAVITY * dt);
}
