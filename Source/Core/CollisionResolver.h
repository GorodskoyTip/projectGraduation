#pragma once
#include "axmol.h"

struct PhysicsBody
{
    ax::Vec2 velocity = ax::Vec2::ZERO;
    bool onGround     = false;

    virtual ax::Rect getPhysicsRect() const = 0;
    virtual ax::Vec2 getPosition() const    = 0;
    virtual void setPosition(const ax::Vec2 pos) = 0;
};
