#include "Player.h"
#include "Core/LevelBounds.h"
#include <algorithm>

USING_NS_AX;

static constexpr float MOVE_SPEED        = 200.0f;
static constexpr float AIR_MOVE_SPEED    = 80.0f;
static constexpr float AIR_MOVE_FROM_RUN = 200.0f;
static constexpr float AIR_ACCEL         = 600.0f;
static constexpr float JUMP_SPEED        = 420.0f;
static constexpr float GRAVITY           = 900.0f;

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
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Content/Sprites/RedHood/spriteSheetRedHood.plist");

    if (!initWithSpriteFrameName("redHood_idle_1.png"))
    {
        AXLOG("PLAYER SPRITE NOT LOADED");
        return false;
    }

    idleAnim = createAnimation("idle", 0.1f);
    runAnim  = createAnimation("run", 0.035f);
    jumpAnim = createAnimation("jump", 0.05f);
    fallAnim = createAnimation("fall", 0.05f);
    lightAttack1Anim = createAnimation("lightAttackFirstHit", 0.07);
    lightAttack2Anim = createAnimation("lightAttackSecondHit", 0.07);
    lightAttack3Anim = createAnimation("lightAttackThirdHit", 0.07);
    heavyAttack1Anim = createAnimation("heavyAttackFirstHit", 0.07);
    heavyAttack2Anim = createAnimation("heavyAttackSecondHit", 0.07);
    heavyAttack3Anim = createAnimation("heavyAttackThirdHit", 0.07);
    hitAnim = createAnimation("damageReceived", 0.1);
    deathAnim = createAnimation("dead", 0.05);

    if (!idleAnim || !runAnim || !jumpAnim || !fallAnim || !lightAttack1Anim || !lightAttack2Anim || !lightAttack3Anim)
    {
        AXLOG("ANIMATION CREATION FAILED");
        return false;
    }
    
    setVelocityX(0);
    setVelocityY(0);

    AXLOG("PLAYER INIT OK");
    return true;
}

void Player::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
    if (state == PlayerState::Dead)
    {
        if (key == EventKeyboard::KeyCode::KEY_R)
        {
            hp = 100;
            setPosition(200, 240);
            state = PlayerState::Idle;
        }
    }

    if (state == PlayerState::Hit || state == PlayerState::Dead)
        return;

    if (key == EventKeyboard::KeyCode::KEY_A)
        moveLeft = true;
    if (key == EventKeyboard::KeyCode::KEY_D)
        moveRight = true;

    if (key == EventKeyboard::KeyCode::KEY_SPACE)
    {
        if (isOnGround() && (state == PlayerState::Idle || state == PlayerState::Run))
        {
            setVelocityY(JUMP_SPEED);
            setOnGround(false);

            jumpFromRun = (state == PlayerState::Run);
            state       = PlayerState::Jump;
        }
    }

    if (key == EventKeyboard::KeyCode::KEY_I)
    {
        if (!attackButtonHeld)
        {
            attackButtonHeld = true;

            if (state != PlayerState::LightAttack && state != PlayerState::HeavyAttack)
                startAttack(PlayerState::LightAttack, 0);
            else
            {
                comboQueued      = true;
                queuedAttackType = PlayerState::LightAttack;
            }
        }
    }

    if (key == EventKeyboard::KeyCode::KEY_J)
    {
        if (!attackButtonHeld)
        {
            attackButtonHeld = true;

            if (state != PlayerState::LightAttack && state != PlayerState::HeavyAttack)
                startAttack(PlayerState::HeavyAttack, 0);
            else
            {
                comboQueued      = true;
                queuedAttackType = PlayerState::HeavyAttack;
            }
        }
    }
}

void Player::onKeyReleased(EventKeyboard::KeyCode key, Event*)
{
    if (key == EventKeyboard::KeyCode::KEY_A) moveLeft = false;
    if (key == EventKeyboard::KeyCode::KEY_D) moveRight = false;
    if (key == EventKeyboard::KeyCode::KEY_I) attackButtonHeld = false;
    if (key == EventKeyboard::KeyCode::KEY_J) attackButtonHeld = false;
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
    constexpr float W = 25.0f;
    constexpr float H = 30.0f;

    auto pos = getPosition();

    return ax::Rect(pos.x - W / 2, pos.y - H, W, H);
}

void Player::updateAnimation()
{
    bool attackState = state == PlayerState::LightAttack || state == PlayerState::HeavyAttack;

    if (!attackState)
    {
        if (state == currentAnimationState)
            return;
    }
    else
    {
        if (state == currentAnimationState && lastAttackComboIndex == comboIndex)
            return;
    }

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

    case PlayerState::Hit:
        runAction(Animate::create(hitAnim));
        break;

    case PlayerState::Dead:
        runAction(Animate::create(deathAnim));
        break;

    case PlayerState::LightAttack:
    {
        ax::Animation* anim = nullptr;

        if (comboIndex == 0)
            anim = lightAttack1Anim;
        else if (comboIndex == 1)
            anim = lightAttack2Anim;
        else if (comboIndex == 2)
            anim = lightAttack3Anim;

        if (anim)
            runAction(Animate::create(anim));

        lastAttackComboIndex = comboIndex;
        break;
    }

    case PlayerState::HeavyAttack:
    {
        ax::Animation* anim = nullptr;

        if (comboIndex == 0)
            anim = heavyAttack1Anim;
        else if (comboIndex == 1)
            anim = heavyAttack2Anim;
        else if (comboIndex == 2)
            anim = heavyAttack3Anim;

        if (anim)
            runAction(Animate::create(anim));

        lastAttackComboIndex = comboIndex;
        break;
    }
    }

    currentAnimationState = state;
}

ax::Animation* Player::createAnimation(const std::string& prefix, float delay)
{
    Vector<SpriteFrame*> frames;
    int index = 1;

    while (true)
    {
        std::string name   = "redHood_" + prefix + "_" + std::to_string(index) + ".png";
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

float Player::getHP()
{
    return hp;
}

void Player::startAttack(PlayerState attackType, int index)
{
    attackID++;

    comboIndex       = index;
    state            = attackType;
    attackActive     = true;

    ax::Animation* anim = nullptr;

    if (attackType == PlayerState::LightAttack)
    {
        if (index == 0)
            anim = lightAttack1Anim;
        else if (index == 1)
            anim = lightAttack2Anim;
        else if (index == 2)
            anim = lightAttack3Anim;
    }
    else if (attackType == PlayerState::HeavyAttack)
    {
        if (index == 0)
            anim = heavyAttack1Anim;
        else if (index == 1)
            anim = heavyAttack2Anim;
        else if (index == 2)
            anim = heavyAttack3Anim;
    }

    if (anim)
    {
        attackTimer   = anim->getDuration();
        attackElapsed = 0.f;
    }

    if (attackType == PlayerState::LightAttack)
    {
        attackActiveStart = attackTimer * 0.45f;
        attackActiveEnd   = attackTimer * 0.85f;
    }
    else if (attackType == PlayerState::HeavyAttack)
    {
        attackActiveStart = attackTimer * 0.55f;
        attackActiveEnd   = attackTimer * 0.85f;
    } 
}

void Player::updateAttack(float dt)
{
    if (state != PlayerState::LightAttack && state != PlayerState::HeavyAttack)
        return;

    attackElapsed += dt;

    if (attackElapsed >= attackActiveStart && attackElapsed <= attackActiveEnd)
    {
        attackActive = true;

        float width  = 40.f;
        float height = 30.f;

        auto pos      = getPosition();
        float offsetX = facingRight ? -20.f : 30.f;

        hitBox = ax::Rect(pos.x + offsetX - width / 1.5, pos.y - height, width, height);

        float attackMoveSpeed = (state == PlayerState::HeavyAttack) ? 20.f : 20.f;
        setVelocityX(facingRight ? -attackMoveSpeed : attackMoveSpeed);
    }
    else
        attackActive = false;
}

int Player::getAttackDamage(PlayerState attackType)
{
    if (attackType == PlayerState::LightAttack)
        return lightAttackDamage;
    else if (attackType == PlayerState::HeavyAttack)
        return heavyAttackDamage;
}

void Player::receiveDamage(float amount)
{
    if (isInvincible || state == PlayerState::Dead)
        return;

    hp                 = std::max(0.0f, hp - amount);
    state              = PlayerState::Hit;
    hitTimer           = 0.5;
    isInvincible       = true;
    invincibilityTimer = 1;

    attackActive = false;
    comboQueued  = false;
    inRecovery   = false;

    stopAllActions();

    AXLOG("Damage received: %f", amount);

    if (hp <= 0)
        onDeath();
}

void Player::onDeath()
{
    state = PlayerState::Dead;

    setVelocityX(0);
    setVelocityY(0);

    attackActive = false;
    comboQueued  = false;
    inRecovery   = false;

    deathTimer = 2.0f;

    stopAllActions();
}

ax::Rect Player::getHurtBox() const
{
    constexpr float W = 20.0f;
    constexpr float H = 25.0f;

    auto pos = getPosition();
    return ax::Rect(pos.x - W / 2, pos.y - H, W, H);
}

void Player::handleIdle(float dt)
{
    setVelocityX(0);

    if (moveLeft || moveRight)
        state = PlayerState::Run;

    if (!isOnGround())
        state = PlayerState::Fall;
}

void Player::handleRun(float dt)
{
    if (moveLeft)
        setVelocityX(-MOVE_SPEED);
    else if (moveRight)
        setVelocityX(MOVE_SPEED);
    else
    {
        setVelocityX(0);
        state      = PlayerState::Idle;
    }

    if (!isOnGround())
    {
        jumpFromRun = true;
        state       = PlayerState::Fall;
    }
}

void Player::handleJump(float dt)
{
    handleAirMovement(dt);

    if (getVelocity().y <= 0)
        state = PlayerState::Fall;
}

void Player::handleFall(float dt)
{
    handleAirMovement(dt);

    if (isOnGround())
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
            setVelocityX(-AIR_MOVE_SPEED);
        else if (moveRight)
            setVelocityX(AIR_MOVE_SPEED);
    }
    else
    {
        float vx = getVelocity().x;

        if (moveLeft)
            vx -= AIR_ACCEL * dt;
        else if (moveRight)
            vx += AIR_ACCEL * dt;

        vx = std::clamp(vx, -AIR_MOVE_FROM_RUN, AIR_MOVE_FROM_RUN);
        setVelocityX(vx);
    }
}

void Player::handleAttack(float dt)
{
    if (isOnGround() && !attackActive)
        setVelocityX(0);

    // Если идёт сама атака
    if (!inRecovery)
    {
        attackTimer -= dt;

        // Атака закончилась
        if (attackTimer <= 0.f)
        {
            attackActive = false;

            // Если в очереди следующая атака
            if (comboQueued && comboIndex < 2)
            {
                comboQueued = false;
                startAttack(queuedAttackType, comboIndex + 1);
                return;
            }

            // Иначе — уходим в recovery
            comboQueued   = false;
            inRecovery    = true;
            recoveryTimer = 0.25f;  // ← вот твоя "пауза в последнем кадре"
        }
    }
    else
    {
        if (comboQueued && comboIndex < 2)
        {
            comboQueued = false;
            inRecovery  = false;
            startAttack(queuedAttackType, comboIndex + 1);
            return;
        }

        recoveryTimer -= dt;

        if (recoveryTimer <= 0.0f)
        {
            inRecovery = false;
            comboQueued = false;
            comboIndex  = 0;

            if (!isOnGround())
                state = PlayerState::Fall;
            else if (moveLeft || moveRight)
                state = PlayerState::Run;
            else
                state = PlayerState::Idle;
        }
    }
}

void Player::handleHit(float dt)
{
    if (isOnGround())
        setVelocityX(0);

    hitTimer -= dt;

    if (hitTimer <= 0)
    {
        if (!isOnGround())
            state = PlayerState::Fall;
        else if (moveLeft || moveRight)
            state = PlayerState::Run;
        else
            state = PlayerState::Idle;
    }
}

void Player::handleDead(float dt)
{
    setVelocityX(0);
    deathTimer -= dt;
}

void Player::updateFacingDirection()
{
    if (getVelocity().x < 0.0f && !facingRight)
    {
        setFlippedX(false);
        facingRight = true;
    }

    // Если движемся влево
    else if (getVelocity().x > 0.0f && facingRight)
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
        handleIdle(dt);
        break;
    case PlayerState::Run:
        handleRun(dt);
        break;
    case PlayerState::Jump:
        handleJump(dt);
        break;
    case PlayerState::Fall:
        handleFall(dt);
        break;
    case PlayerState::LightAttack:
        handleAttack(dt);
        break;
    case PlayerState::HeavyAttack:
        handleAttack(dt);
        break;
    case PlayerState::Hit:
        handleHit(dt);
        break;
    case PlayerState::Dead:
        handleDead(dt);
        break;
    }

    updateFacingDirection();
    updateAnimation();
    updateAttack(dt);

    if (invincibilityTimer > 0)
        invincibilityTimer -= dt;
    else if (invincibilityTimer <= 0)
    {
        invincibilityTimer = 0;
        isInvincible = false;
    }

    setVelocityY(getVelocity().y - GRAVITY * dt);

}
