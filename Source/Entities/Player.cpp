#include "Player.h"
#include "Core/LevelBounds.h"

USING_NS_AX;

static constexpr float MOVE_SPEED = 200.0f;
static constexpr float JUMP_SPEED = 420.0f;
static constexpr float GRAVITY    = -900.0f;

Player* Player::create()
{
    Player* p = new (std::nothrow) Player();
    if (p && p->init())
    {
        p->autorelease();
        return p;
    }
    AX_SAFE_DELETE(p);
    return nullptr;
}

bool Player::init()
{
    if (!Sprite::initWithFile("Content/frames/idle_anim/idle_anim_1.png"))
    {
        AXLOG("PLAYER SPRITE NOT LOADED");
        return false;
    }

    AXLOG("PLAYER INIT OK");
    velocity = Vec2::ZERO;

    return true;
}

void Player::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
    if (key == EventKeyboard::KeyCode::KEY_A) moveLeft = true;
    if (key == EventKeyboard::KeyCode::KEY_D) moveRight = true;

    if (key == EventKeyboard::KeyCode::KEY_SPACE && onGround)
    {
        velocity.y = JUMP_SPEED;
        onGround   = false;
    }

    AXLOG("KEY PRESSED: %d", (int)key);
}

void Player::onKeyReleased(EventKeyboard::KeyCode key, Event*)
{
    if (key == EventKeyboard::KeyCode::KEY_A) moveLeft = false;
    if (key == EventKeyboard::KeyCode::KEY_D) moveRight = false;
}

void Player::update(float dt)
{
    if (moveLeft)
        velocity.x = -MOVE_SPEED;
    else if (moveRight)
        velocity.x = MOVE_SPEED;
    else
        velocity.x = 0;

    velocity.y += GRAVITY * dt;
}

void Player::onEnter()
{
    Sprite::onEnter();

    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed  = AX_CALLBACK_2(Player::onKeyPressed, this);
    listener->onKeyReleased = AX_CALLBACK_2(Player::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Player::onExit()
{
    _eventDispatcher->removeEventListenersForTarget(this);
    Sprite::onExit();
}

float Player::getDirection() const
{
    if (velocity.x > 0)
        return 1.0f;
    if (velocity.x < 0)
        return -1.0f;
    return 0.0f;
}

ax::Rect Player::getPhysicsRect() const
{
    constexpr float PHYS_WIDTH = 40.0f;
    constexpr float PHYS_HEIGHT = 80.0f;

    auto pos = getPosition();
    return ax::Rect(pos.x - PHYS_WIDTH / 2, pos.y - PHYS_HEIGHT / 2, PHYS_WIDTH, PHYS_HEIGHT);
}

void Player::setOnGround(bool value)
{
    onGround = value;
}

bool Player::isOnGround() const
{
    return onGround;
}
