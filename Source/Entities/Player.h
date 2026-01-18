#pragma once
#include "axmol.h"

class Player : public ax::Sprite
{
public:

    ax::Vec2 velocity;

    static Player* create();
    virtual bool init() override;
    virtual void update(float dt) override;

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    virtual void onEnter() override;
    virtual void onExit() override;

    float getDirection() const;

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);
    bool isOnGround() const;

private:

    bool moveLeft  = false;
    bool moveRight = false;
    bool onGround  = false;
};
