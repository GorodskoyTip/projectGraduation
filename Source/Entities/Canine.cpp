#include "Canine.h"
#include "Player.h"

static constexpr float ENEMY_MOVE_SPEED = 150.0f;

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
    setAnchorPoint(ax::Vec2(0.5f, 0.5f));

    idleAnim   = createAnimation("wolfBlack", "idle", 0.1f);
    runAnim    = createAnimation("wolfBlack", "run", 0.07f);
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
    deaggroRange   = 600.f;
    attackRange    = 40.f;
    attackDamage   = 10;
    attackDuration = attackAnim->getDuration();
    hp             = 50.f;

    AXLOG("CANINE INIT OK");
    return true;
}

void Canine::handleIdle(float dt)
{
    setVelocityX(0);

    if (!onGround)
        state = EnemyState::Fall;
}

void Canine::handleMove(float dt)
{
    float dx = target->getPositionX() - getPositionX();

    constexpr float TURN_THRESHOLD = 10.f;

    if (std::abs(dx) > TURN_THRESHOLD)
    {
        if (dx > 0)
        {
            setVelocityX(ENEMY_MOVE_SPEED);
            setFlippedX(true);
            facingRight = false;
        }
        else
        {
            setVelocityX(-ENEMY_MOVE_SPEED);
            setFlippedX(false);
            facingRight = true;
        }
    }
    else
    {
        setVelocityX(0);
    }
}

void Canine::handleFall(float dt) {}

void Canine::handleAttack(float dt)
{
    setVelocityX(0);

    attackTimer -= dt;

    if (attackTimer <= 0)
        state = EnemyState::Idle;
}

void Canine::handleHit(float dt)
{
    if (onGround)
        setVelocityX(0);

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
    setVelocityX(0);
    setVelocityY(0);

    if (deathTimer > 0)
        deathTimer -= dt;
}

void Canine::updateAI(float dt)
{
    if (!target)
        return;

    if (state == EnemyState::Attack || state == EnemyState::Hit || state == EnemyState::Dead)
        return;

    if (attackCooldown > 0)
        attackCooldown -= dt;

    float dx = target->getPositionX() - getPositionX();
    float dy = target->getPositionY() - getPositionY();

    float absDx = std::abs(dx);
    float absDy = std::abs(dy);

    constexpr float HEIGHT_TOLERANCE = 5.f;
    constexpr float ATTACK_HEIGHT      = 20.f;
    constexpr float CHASE_HEIGHT_LIMIT = 60.f;
    constexpr float UNDER_PLAYER_X     = 30.f;

    if (!isAggro && absDx <= aggroRange && absDy <= HEIGHT_TOLERANCE)
        isAggro = true;

    if (isAggro && absDx > deaggroRange)
    {
        isAggro = false;
        state   = EnemyState::Idle;
        return;
    }

    if (!isAggro)
        return;

    if (absDx <= attackRange && absDy <= ATTACK_HEIGHT)
    {
        if (attackCooldown <= 0)
            startAttack();
        return;
    }

    if (dy > 0 && target->isOnGround() && absDx <= UNDER_PLAYER_X)
    {
        setVelocityX(0);

        if (dx > 0)
        {
            setFlippedX(true);
            facingRight = false;
        }
        else
        {
            setFlippedX(false);
            facingRight = true;
        }

        state = EnemyState::Idle;
        return;
    }

    state = EnemyState::Move;
}

void Canine::startAttack()
{
    if (attackCooldown > 0 || hitTimer > 0)
        return;

    state = EnemyState::Attack;

    attackActive = false;
    attackTimer  = attackDuration;

    attackCooldown = 2.0f;

    attackElapsed     = 0.f;
    attackActiveStart = attackDuration * 0.55f;
    attackActiveEnd   = attackDuration * 0.7f;
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
        float offsetX = facingRight ? -25.0f : 25.0f;

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
