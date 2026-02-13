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

    ax::Vec2 velocity;

    static Player* create();
    virtual bool init() override;
    virtual void update(float dt) override;

    PlayerState getState() const { return state; }

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    virtual void onEnter() override;
    virtual void onExit() override;

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);
    bool isOnGround() const;

    float getHP();

    void receiveDamage(int amount);

private:
    void updateIdle(float dt);
    void updateRun(float dt);
    void updateJump(float dt);
    void updateFall(float dt);
    void handleAirMovement(float dt);

    PlayerState state = PlayerState::Idle;
    bool jumpFromRun  = false;

    bool moveLeft  = false;
    bool moveRight = false;
    bool onGround  = false;

    float invincibilityTimer;
    bool isInvincible;

    float hp = 100;
};
