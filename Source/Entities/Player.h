#pragma once
#include "axmol.h"
#include "Core/CollisionResolver.h"

enum class PlayerState
{
    Idle,
    Run,
    Jump,
    Fall
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

    void receiveDamage(int amount);

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

    bool facingRight = true;
    void updateFacingDirection();
    ax::Animation* idleAnim;
    ax::Animation* runAnim;
    ax::Animation* jumpAnim;
    ax::Animation* fallAnim;
    PlayerState currentAnimationState = PlayerState::Idle;

    float hp = 100;

    float invincibilityTimer;
    bool isInvincible;
};
