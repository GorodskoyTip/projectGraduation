#include "Boss.h"
#include "Player.h"

USING_NS_AX;

static constexpr float GRAVITY = 900.f;

bool Boss::initBase()
{
    setVelocityX(0);
    setVelocityY(0);
    state    = BossState::Idle;
    return true;
}

void Boss::setArena(const ax::Rect& rect)
{
    arena = rect;
}

ax::Rect Boss::getPhysicsRect() const
{
    constexpr float W = 60;
    constexpr float H = 60;

    auto pos = getPosition();

    return ax::Rect(pos.x - W / 2, pos.y - H, W, H);
}

void Boss::receiveDamage(float amount, int attackID)
{
    if (lastReceivedAttackID == attackID)
        return;

    lastReceivedAttackID = attackID;

    hp    = std::max(0.0f, hp - amount);
    //state = BossState::Hit;

    AXLOG("Damage received by boss: %f", amount);

    if (hp <= 0)
        onDeath();
}

void Boss::onDeath()
{
    state = BossState::Dead;
    deathTimer = 2.f;
    stopAllActions();
}

ax::Animation* Boss::createAnimation(const std::string& entity, const std::string& prefix, float delay)
{
    Vector<SpriteFrame*> frames;
    int index = 1;

    while (true)
    {
        std::string name = entity + "_" + prefix + "_" + std::to_string(index) + ".png";
        auto frame       = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);

        if (!frame)
            break;

        frames.pushBack(frame);
        index++;
    }

    if (frames.empty())
        return nullptr;

    auto anim = Animation::createWithSpriteFrames(frames, delay);
    anim->retain();
    return anim;
}

void Boss::update(float dt)
{
    if (state == BossState::Dead)
    {
        deathTimer -= dt;
        updateAnimation();
        return;
    }

    if (!fightStarted)
    {
        if (!target)
            return;
        if (arena.containsPoint(target->getPosition()))
            fightStarted = true;
        else
            return;
    }

    if (attackCooldown > 0)
        attackCooldown -= dt;

    updateAI(dt);

    switch (state)
    {
    case BossState::Idle:
        handleIdle(dt);
        break;
    case BossState::Walk:
        handleMove(dt);
        break;
    case BossState::Run:
        handleMove(dt);
        break;
    case BossState::Jump:
        handleJump(dt);
        break;
    case BossState::Attack:
        handleAttack(dt);
        break;
    case BossState::Hit:
        handleHit(dt);
        break;
    default:
        break;
    }

    updateAttack(dt);
    updateAnimation();

    setVelocityY(getVelocity().y - GRAVITY * dt);

    static float logTimer = 0.f;
    logTimer += dt;

    if (logTimer >= 0.5f)  // раз в полсекунды
    {
        auto pos = getPosition();
        AXLOG("Boss position: x=%.2f y=%.2f", pos.x, pos.y);
        logTimer = 0.f;
    }
}
