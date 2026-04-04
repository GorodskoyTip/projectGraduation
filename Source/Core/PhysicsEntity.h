#pragma once
#include "axmol.h"

class PhysicsEntity : public ax::Sprite
{
public:

    ax::Vec2 getVelocity() const { return velocity; }
    void setVelocityX(float x) { velocity.x = x; }
    void setVelocityY(float y) { velocity.y = y; }

    virtual ax::Rect getPhysicsRect() const = 0;

    bool isOnGround() { return onGround; }
    void setOnGround(bool value) { onGround = value; }

    virtual ~PhysicsEntity() = default;

protected:
    ax::Vec2 velocity;
    bool onGround;
};
