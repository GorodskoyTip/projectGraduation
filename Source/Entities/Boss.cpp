#include "Boss.h"
#include "Player.h"

USING_NS_AX;

static constexpr float GRAVITY = -900.f;

bool Boss::initBase()
{
    velocity = ax::Vec2::ZERO;
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

void Boss::receiveDamage(float amount)
{
    if (state == BossState::Dead)
        return;

    hp -= amount;

    if (hp <= 0)
    {
        state      = BossState::Dead;
        velocity   = ax::Vec2::ZERO;
        deathTimer = 2.0f;
    }
    else
        state = BossState::Hit;

    AXLOG("Damage received by Boss: %f", amount);
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

    //updateAI(dt);

    switch (state)
    {
    case BossState::Idle:
        handleIdle(dt);
        break;
    case BossState::Move:
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

    if (!onGround)
        velocity.y += GRAVITY * dt;

    velocity.y = std::max(velocity.y, -1500.f);

    static float logTimer = 0.f;
    logTimer += dt;

    if (logTimer >= 0.5f)  // раз в полсекунды
    {
        auto pos = getPosition();
        AXLOG("Boss position: x=%.2f y=%.2f", pos.x, pos.y);
        logTimer = 0.f;
    }
}
