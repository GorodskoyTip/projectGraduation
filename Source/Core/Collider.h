#pragma once

#include "axmol.h"

struct Collider
{
    ax::Rect rect;

    Collider(const ax::Rect& r) : rect(r) {}

    bool intersects(const Collider& other) const
    { return rect.intersectsRect(other.rect); }
};
