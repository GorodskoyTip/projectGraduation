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

    idleAnim = createAnimation("wolfBlack", "idle", 0.1f);
    runAnim  = createAnimation("wolfBlack", "run", 0.035f);
    fallAnim = createAnimation("wolfBlack", "idle", 0.1f);
    deathAnim = createAnimation("wolfBlack", "death", 0.035f);

    if (!idleAnim || !runAnim || !fallAnim || !deathAnim)
    {
        AXLOG("ANIMATION CREATION FAILED");
        return false;
    }

    aggroRange  = 400.f;
    attackRange = 60.f;
    hp          = 50.f;

    AXLOG("CANINE INIT OK");
    return true;
}

void Canine::handleIdle(float dt)
{
    velocity.x = 0;

    if (!onGround)
        state = EnemyState::Fall;
}

void Canine::handleMove(float dt) {
}

void Canine::handleFall(float dt) {}

void Canine::handleDeath(float dt) {}

void Canine::updateAI(float dt) {}

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
    case EnemyState::Dead:
        runAction(RepeatForever::create(Animate::create(deathAnim)));
        break;
    }

    currentAnimationState = state;
}
