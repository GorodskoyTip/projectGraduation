#pragma once
#include "axmol.h"
#include "Core/CollisionResolver.h"

enum class PlayerState
{
    Idle,
    Run,
    Jump,
    Fall,
    Attack
};

class Player : public ax::Sprite
{
public:
    static Player* create();
    virtual bool init() override;
    virtual void update(float dt) override;

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    virtual void onEnter() override;
    virtual void onExit() override;

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);
    bool isOnGround() const;
    ax::Vec2 getVelocity();
    void setVelocityX(float x);
    void setVelocityY(float y);

    PlayerState getState() const { return state; }
    void updateAnimation();
    ax::Animation* createAnimation(const std::string& prefix, float delay);

    float getHP();

    ax::Rect getHurtBox() const;
    void receiveDamage(float amount);

    void updateAttack(float dt);
    ax::Rect getHitBox() { return hitBox; }
    bool isAttackActive() { return attackActive; }
    int getAttackDamage() { return attackDamage; }

    int getAttackID() const { return attackID; }

private:
    PlayerState state = PlayerState::Idle;
    bool jumpFromRun  = false;
    ax::Vec2 velocity;

    bool moveLeft  = false;
    bool moveRight = false;
    bool onGround  = false;

    void handleIdle(float dt);
    void handleRun(float dt);
    void handleJump(float dt);
    void handleFall(float dt);
    void handleAirMovement(float dt);
    void handleAttack(float dt);

    bool facingRight = true;
    void updateFacingDirection();
    ax::Animation* idleAnim;
    ax::Animation* runAnim;
    ax::Animation* jumpAnim;
    ax::Animation* fallAnim;
    ax::Animation* lightAttack1Anim;
    ax::Animation* lightAttack2Anim;
    ax::Animation* lightAttack3Anim;
    PlayerState currentAnimationState = PlayerState::Idle;

    float hp = 100;

    float invincibilityTimer;
    bool isInvincible;

    ax::Rect hitBox;
    bool attackButtonHeld = false;
    bool attackActive     = false;
    float attackTimer     = 0.f;
    int attackDamage      = 10;

    int comboIndex         = 0;
    int lastComboIndex     = -1;
    bool comboQueued       = false;
    void startAttack(int index);
    float recoveryTimer = 0.f;
    bool inRecovery      = false;

    int attackID = 0;
};
