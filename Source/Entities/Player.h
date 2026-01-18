#pragma once
#include "axmol.h"

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

private:
    PlayerState state = PlayerState::Idle;
    bool jumpFromRun  = false;

    bool moveLeft  = false;
    bool moveRight = false;
    bool onGround  = false;
};
