#include "Canine.h"

USING_NS_AX;

Canine* Canine::create()
{
    Canine* c = new (std::nothrow) Canine();
    if (c && c->init())
    {
        c->autorelease();
        return c;
    }
    AX_SAFE_DELETE(c);
    return nullptr;
}

bool Canine::init()
{
    if (!Sprite::init())
        return false;

    if (!initBase())
        return false;

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Content/Sprites/WolfBlack/spriteSheetWolfBlack.plist");

    if (!initWithSpriteFrameName("wolfBlack_idle_1.png"))
    {
        AXLOG("ENEMY SPRITE NOT LOADED");
        return false;
    }

    idleAnim   = createAnimation("wolfBlack", "idle", 0.1f);
    runAnim    = createAnimation("wolfBlack", "run", 0.035f);
    fallAnim   = createAnimation("wolfBlack", "idle", 0.1f);
    attackAnim = createAnimation("wolfBlack", "attack", 0.1f);
    hitAnim    = createAnimation("wolfBlack", "hit", 0.035f);
    deathAnim  = createAnimation("wolfBlack", "death", 0.035f);

    if (!idleAnim || !runAnim || !fallAnim || !hitAnim || !deathAnim)
    {
        AXLOG("ANIMATION CREATION FAILED");
        return false;
    }

    aggroRange     = 400.f;
    attackRange    = 60.f;
    attackDamage   = 10;
    attackDuration = attackAnim->getDuration();
    hp             = 50.f;

    AXLOG("CANINE INIT OK");
    return true;
}

void Canine::handleIdle(float dt)
{
    velocity.x = 0;

    if (!onGround)
        state = EnemyState::Fall;
}

void Canine::handleMove(float dt) {}

void Canine::handleFall(float dt) {}

void Canine::handleAttack(float dt)
{
    velocity.x = 0;

    attackTimer -= dt;

    if (attackTimer <= 0)
        state = EnemyState::Idle;
}

void Canine::handleHit(float dt)
{
    if (onGround)
        velocity.x = 0;

    hitTimer -= dt;

    if (hitTimer <= 0.0f)
    {
        if (!onGround)
            state = EnemyState::Fall;
        else
            state = EnemyState::Idle;
    }
}

void Canine::handleDeath(float dt)
{
    velocity = Vec2::ZERO;

    if (deathTimer > 0)
        deathTimer -= dt;
}

void Canine::updateAI(float dt)
{
    if (attackCooldown > 0)
        attackCooldown -= dt;

    if (state == EnemyState::Idle)
        startAttack();
}

void Canine::startAttack()
{
    if (attackCooldown > 0)
        return;

    state = EnemyState::Attack;

    attackActive = true;
    attackTimer  = attackDuration;

    attackCooldown = 1.2f;

    attackElapsed     = 0.f;
    attackActiveStart = attackDuration * 0.55f;
    attackActiveEnd   = attackDuration * 0.8f;
}

void Canine::updateAttack(float dt)
{
    attackElapsed += dt;

if (attackElapsed >= attackActiveStart && attackElapsed <= attackActiveEnd)
    {
        attackActive = true;

        float width  = 10.0f;
        float height = 15.0f;

        auto pos      = getPosition();
        float offsetX = facingRight ? 25.0f : -25.0f;

        hitBox = ax::Rect(pos.x + offsetX - width / 2, pos.y - height, width, height);
    }
    else
        attackActive = false;
}

void Canine::updateAnimation()
{
    if (state == currentAnimationState)
        return;

    stopAllActions();

    switch (state)
    {
    case EnemyState::Idle:
        runAction(RepeatForever::create(Animate::create(idleAnim)));
        break;
    case EnemyState::Move:
        runAction(RepeatForever::create(Animate::create(runAnim)));
        break;
    case EnemyState::Fall:
        runAction(RepeatForever::create(Animate::create(fallAnim)));
        break;
    case EnemyState::Attack:
        runAction(Animate::create(attackAnim));
        break;
    case EnemyState::Hit:
        runAction(Animate::create(hitAnim));
        break;
    case EnemyState::Dead:
        runAction(Animate::create(deathAnim));
        break;
    }

    currentAnimationState = state;
}
