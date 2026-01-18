#pragma once

#include "axmol.h"

enum class ColliderType
{
    Solid,
    OneWay
};

struct Collider
{
    ax::Rect rect;
    ColliderType type;

    Collider(const ax::Rect& r, ColliderType t) : rect(r), type(t) {}

    bool intersects(const Collider& other) const
    { return rect.intersectsRect(other.rect); }
};
