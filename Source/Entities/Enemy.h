#pragma once
#include "axmol.h"

class Enemy : public ax::Sprite
{
public:
    static Enemy* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void changeDirection();

    ax::Rect getPhysicsRect() const;
    void setOnGround(bool value);

    ax::Vec2 velocity;

private:
    int direction = -1;
    bool onGround = false;
};
