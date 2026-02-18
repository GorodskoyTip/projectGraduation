#include "Player.h"
#include "Core/LevelBounds.h"
#include <algorithm>

USING_NS_AX;

static constexpr float MOVE_SPEED        = 200.0f;
static constexpr float AIR_MOVE_SPEED    = 80.0f;
static constexpr float AIR_MOVE_FROM_RUN = 200.0f;
static constexpr float AIR_ACCEL         = 600.0f;
static constexpr float JUMP_SPEED        = 420.0f;
static constexpr float GRAVITY           = -900.0f;

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
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Content/Sprites/RedHood/spriteSheet.plist");

    if (!initWithSpriteFrameName("idle_1.png"))
    {
        AXLOG("PLAYER SPRITE NOT LOADED");
        return false;
    }

    idleAnim = createAnimation("idle", 0.1f);
    runAnim  = createAnimation("run", 0.035f);
    jumpAnim = createAnimation("jump", 0.05f);
    fallAnim = createAnimation("fall", 0.05f);

    if (!idleAnim || !runAnim)
    {
        AXLOG("ANIMATION CREATION FAILED");
        return false;
    }
    
    AXLOG("PLAYER INIT OK");
    velocity = Vec2::ZERO;

    return true;
}

void Player::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
    if (key == EventKeyboard::KeyCode::KEY_A)
        moveLeft = true;
    if (key == EventKeyboard::KeyCode::KEY_D)
        moveRight = true;

    if (key == EventKeyboard::KeyCode::KEY_SPACE)
    {
        if (onGround && (state == PlayerState::Idle || state == PlayerState::Run))
        {
            velocity.y = JUMP_SPEED;
            onGround   = false;

            jumpFromRun = (state == PlayerState::Run);
            state = PlayerState::Jump;
        }
    }
}

void Player::onKeyReleased(EventKeyboard::KeyCode key, Event*)
{
    if (key == EventKeyboard::KeyCode::KEY_A) moveLeft = false;
    if (key == EventKeyboard::KeyCode::KEY_D) moveRight = false;
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

ax::Rect Player::getPhysicsRect() const
{
    constexpr float PHYS_WIDTH  = 40.0f;
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

ax::Vec2 Player::getVelocity()
{
    return velocity;
}

void Player::setVelocityX(float x)
{
    velocity.x = x;
}

void Player::setVelocityY(float y)
{
    velocity.y = y;
}

void Player::updateAnimation()
{
    if (state == currentAnimationState)
        return;

    stopAllActions();

    switch (state)
    {
    case PlayerState::Idle:
        runAction(RepeatForever::create(Animate::create(idleAnim)));
        break;
    case PlayerState::Run:
        runAction(RepeatForever::create(Animate::create(runAnim)));
        break;
    case PlayerState::Jump:
        runAction(Animate::create(jumpAnim));
        break;
    case PlayerState::Fall:
        runAction(RepeatForever::create(Animate::create(fallAnim)));
        break;
    }
    currentAnimationState = state;
}

ax::Animation* Player::createAnimation(const std::string& prefix, float delay)
{
    Vector<SpriteFrame*> frames;
    int index = 1;

    while (true)
    {
        std::string name = prefix + "_" + std::to_string(index) + ".png";
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
    AnimationCache::getInstance()->addAnimation(animation, prefix);
    return animation;
}

float Player::getHP()
{
    return hp;
}

void Player::receiveDamage(int amount)
{
    if (!isInvincible && hp > 0)
    {
        hp = std::max(0.0f, hp-amount);
        isInvincible = true;
        invincibilityTimer = 1;
    }
}

void Player::updateIdle(float dt)
{
    velocity.x = 0;

    if (moveLeft || moveRight)
        state = PlayerState::Run;

    if (!onGround)
        state = PlayerState::Fall;
}

void Player::updateRun(float dt)
{
    if (moveLeft)
        velocity.x = -MOVE_SPEED;
    else if (moveRight)
        velocity.x = MOVE_SPEED;
    else
    {
        velocity.x = 0;
        state      = PlayerState::Idle;
    }

    if (!onGround)
    {
        jumpFromRun = true;
        state       = PlayerState::Fall;
    }
}

void Player::updateJump(float dt)
{
    handleAirMovement(dt);

    if (velocity.y <= 0)
        state = PlayerState::Fall;
}

void Player::updateFall(float dt)
{
    handleAirMovement(dt);

    if (onGround)
    {
        jumpFromRun = false;
        state       = PlayerState::Idle;
    }
}

void Player::handleAirMovement(float dt)
{
    if (!jumpFromRun)
    {
        if (moveLeft)
            velocity.x = -AIR_MOVE_SPEED;
        else if (moveRight)
            velocity.x = AIR_MOVE_SPEED;
    }
    else
    {
        if (moveLeft)
            velocity.x -= AIR_ACCEL * dt;
        else if (moveRight)
            velocity.x += AIR_ACCEL * dt;

        velocity.x = std::clamp(velocity.x, -AIR_MOVE_FROM_RUN, AIR_MOVE_FROM_RUN);
    }
}

void Player::updateFacingDirection()
{
    if (velocity.x < 0.0f && !facingRight)
    {
        setFlippedX(false);
        facingRight = true;
    }

    // Если движемся влево
    else if (velocity.x > 0.0f && facingRight)
    {
        setFlippedX(true);
        facingRight = false;
    }
}

void Player::update(float dt)
{
    switch (state)
    {
    case PlayerState::Idle:
        updateIdle(dt);
        break;
    case PlayerState::Run:
        updateRun(dt);
        break;
    case PlayerState::Jump:
        updateJump(dt);
        break;
    case PlayerState::Fall:
        updateFall(dt);
        break;
    }
    updateFacingDirection();
    updateAnimation();

    if (invincibilityTimer > 0)
        invincibilityTimer -= dt;
    else if (invincibilityTimer <= 0)
    {
        invincibilityTimer = 0;
        isInvincible = false;
    }

    velocity.y += GRAVITY * dt;
}
