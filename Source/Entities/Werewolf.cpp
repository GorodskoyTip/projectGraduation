#include "Werewolf.h"
#include "Player.h"

USING_NS_AX;

Werewolf* Werewolf::create()
{
    Werewolf* w = new (std::nothrow) Werewolf();

    if (w && w->init())
    {
        w->autorelease();
        return w;
    }

    delete w;
    return nullptr;
}

bool Werewolf::init()
{
    if (!Sprite::init())
        return false;

    if (!initBase())
        return false;

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Content/Sprites/WerewolfWhite/spriteSheetWerewolfWhite.plist");

    if (!initWithSpriteFrameName("werewolfWhite_idle_1.png"))
    {
        AXLOG("WEREWOLF SPRITE NOT LOADED");
        return false;
    }
    setAnchorPoint(ax::Vec2(0.5f, 0.5f));

    idleAnim = createAnimation("werewolfWhite", "idle", 0.1f);
    walkAnim = createAnimation("werewolfWhite", "walk", 0.07f);
    runAnim  = createAnimation("werewolfWhite", "run", 0.07f);
    jumpAnim = createAnimation("werewolfWhite", "jump", 0.08f);

    attack1Anim = createAnimation("werewolfWhite", "attack1", 0.1f);
    attack2Anim = createAnimation("werewolfWhite", "attack2", 0.1f);
    attack3Anim = createAnimation("werewolfWhite", "attack3", 0.1f);

    hitAnim   = createAnimation("werewolfWhite", "hurt", 0.05f);
    deathAnim = createAnimation("werewolfWhite", "dead", 0.1f);

    if (!idleAnim || !runAnim || !attack1Anim)
    {
        AXLOG("ANIMATION FAILED");
        return false;
    }

    hp             = 300;
    attackDamage   = 20;
    attackCooldown = 0;

    return true;
}

void Werewolf::updateAnimation()
{
    if (state == currentAnimationState)
    {
        if (state != BossState::Attack)
            return;
        if (lastAttack == currentAttack)
            return;
    }

    stopAllActions();

    switch (state)
    {
    case BossState::Idle:
        runAction(RepeatForever::create(Animate::create(idleAnim)));
        break;
    case BossState::Walk:
        runAction(RepeatForever::create(Animate::create(walkAnim)));
        break;
    case BossState::Run:
        runAction(RepeatForever::create(Animate::create(runAnim)));
        break;
    case BossState::Jump:
        runAction(Animate::create(jumpAnim));
        break;
    case BossState::Attack:
    {
        ax::Animation* anim = nullptr;

        if (currentAttack == WerewolfAttack::Gnaw)
            anim = attack1Anim;
        else if (currentAttack == WerewolfAttack::Strike)
            anim = attack2Anim;
        else if (currentAttack == WerewolfAttack::Uppercut)
            anim = attack3Anim;

        if (anim)
            runAction(Animate::create(anim));

        lastAttack = currentAttack;
        break;
    }
    case BossState::Hit:
        runAction(Animate::create(hitAnim));
        break;
    case BossState::Dead:
        runAction(Animate::create(deathAnim));
        break;
    }

    currentAnimationState = state;
}

void Werewolf::updateAI(float dt)
{
    if (!target || target->getState() == PlayerState::Dead || state == BossState::Attack || state == BossState::Hit)
        return;

    auto myPos = getPosition();
    auto playerPos = target->getPosition();

    float dx = playerPos.x - myPos.x;
    float dy = playerPos.y - myPos.y;

    float absDx = std::abs(dx);

    facingRight = dx > 0;
    setFlippedX(!facingRight);

    if (dy > jumpHeightThresh && onGround && target->isOnGround())
    {
        velocity.y = 500;
        state      = BossState::Jump;
        return;
    }

    if (absDx < attackDistance && attackCooldown <= 0)
    {
        startAttack();
        return;
    }

    if (absDx <= attackDistance && attackCooldown > 0)
    {
        velocity.x = 0;
        state      = BossState::Idle;
        return;
    }

    if (absDx > walkDistance)
    {
        state = BossState::Run;
        velocity.x = (dx > 0) ? 200.f : -200.f;
        return;
    }

    state = BossState::Walk;
    velocity.x = (dx > 0) ? 100.f : -100.f;
}

void Werewolf::handleIdle(float dt)
{
    velocity.x = 0;

    if (!onGround)
    {
        state = BossState::Jump;
        return;
    }
}

void Werewolf::handleMove(float dt)
{
    if (!onGround)
        state = BossState::Jump;
}

void Werewolf::handleJump(float dt)
{
    if (onGround)
    {
        state = BossState::Idle;
        return;
    }

    float dx = target->getPositionX() - getPositionX();
    velocity.x = std::clamp(dx * 2.0f, -300.f, 300.f);
}

void Werewolf::handleHit(float dt)
{
    velocity.x = 0;

    hitTimer -= dt;

    if (hitTimer <= 0)
    {
        if (!onGround)
            state = BossState::Jump;
        else
            state = BossState::Idle;
    }
}

void Werewolf::handleAttack(float dt)
{
    velocity.x = 0;
}

void Werewolf::chooseAttack()
{
    currentAttack = static_cast<WerewolfAttack>(rand() % 3);
}

void Werewolf::startAttack()
{
    chooseAttack();

    state = BossState::Attack;

    attackElapsed = 0.f;
    attackActive  = false;

    if (currentAttack == WerewolfAttack::Gnaw)
    {
        attackTimer       = attack1Anim->getDuration();
        attackActiveStart = attackTimer * 0.4f;
        attackActiveEnd   = attackTimer * 0.7f;
    }
    else if (currentAttack == WerewolfAttack::Strike)
    {
        attackTimer       = attack2Anim->getDuration();
        attackActiveStart = attackTimer * 0.5f;
        attackActiveEnd   = attackTimer * 0.8f;
    }
    else if (currentAttack == WerewolfAttack::Uppercut)
    {
        attackTimer       = attack3Anim->getDuration();
        attackActiveStart = attackTimer * 0.6f;
        attackActiveEnd   = attackTimer * 0.9f;
    }

    attackCooldown = 2.0f;
}

void Werewolf::updateAttack(float dt)
{
    if (state != BossState::Attack)
        return;

    attackElapsed += dt;

    if (attackElapsed >= attackActiveStart && attackElapsed <= attackActiveEnd)
    {
        attackActive = true;

        float width  = 50.f;
        float height = 80.f;

        auto pos      = getPosition();
        float offsetX = facingRight ? 30.f : -30.f;

        hitBox = ax::Rect(pos.x + offsetX - width / 2, pos.y - height / 1.5, width, height);
    }
    else
        attackActive = false;

    attackTimer -= dt;

    if (attackTimer <= 0)
        state = BossState::Idle;
}
