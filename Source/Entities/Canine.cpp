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

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Content/Sprites/CanineBlack/spriteSheetCanineBlack.plist");

    if (!initWithSpriteFrameName("idle_1.png"))
    {
        AXLOG("ENEMY SPRITE NOT LOADED");
        return false;
    }

    idleAnim = createAnimation("idle", 0.1f);
    runAnim  = createAnimation("run", 0.035f);
    fallAnim = createAnimation("run", 0.1f);
    deadAnim = createAnimation("dead", 0.035f);

    aggroRange  = 400.f;
    attackRange = 60.f;
    hp          = 50.f;

    AXLOG("CANINE INIT OK");
    return true;
}

void Canine::handleIdle(float dt) {}

void Canine::handleMove(float dt) {}

void Canine::handleFall(float dt) {}

void Canine::handleDead(float dt) {}

void Canine::updateAI(float dt) {}
