#include "PhysicsSystem.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/Boss.h"

void PhysicsSystem::addCollider(const Collider& col) { colliders.push_back(col); }
const std::vector<Collider>& PhysicsSystem::getColliders() const { return colliders; }

bool PhysicsSystem::hasGroundBelow(const ax::Vec2& point) const
{
    ax::Rect probe(point.x - 2, point.y - 2, 4, 4);

    for (const auto& col : colliders)
    {
        if (col.type == ColliderType::Solid || col.type == ColliderType::OneWay)
        {
            if (probe.intersectsRect(col.rect))
                return true;
        }
    }

    return false;
}

bool PhysicsSystem::hasWallAhead(const ax::Rect& rect, float dir) const
{
    float checkX = (dir > 0) ? rect.getMaxX() + 2 : rect.getMinX() - 2;

    ax::Rect probe(checkX, rect.getMinY(), 2, rect.size.height);

    for (const auto& col : colliders)
    {
        if (col.type == ColliderType::Solid && probe.intersectsRect(col.rect))
            return true;
    }

    return false;
}

void PhysicsSystem::moveAndCollideX(PhysicsEntity* body, float dt)
{
    auto sprite = static_cast<ax::Sprite*>(body);
    if (!sprite)
        return;

    auto pos = sprite->getPosition();
    pos.x += body->getVelocity().x * dt;
    sprite->setPosition(pos);

    for (const auto& col : colliders)
    {
        auto rectX = body->getPhysicsRect();

        if (col.rect.size.height < 200)
            continue;

        if (!rectX.intersectsRect(col.rect))
            continue;

        if (body->getVelocity().x > 0)
        {
            float penetration = rectX.getMaxX() - col.rect.getMinX();
            pos.x -= penetration;
        }
        else if (body->getVelocity().x < 0)
        {
            float penetration = col.rect.getMaxX() - rectX.getMinX();
            pos.x += penetration;
        }

        sprite->setPosition(pos);
        body->setVelocityX(0);
        break;
    }
}

void PhysicsSystem::moveAndCollideY(PhysicsEntity* body, float dt)
{
    auto sprite = static_cast<ax::Sprite*>(body);
    if (!sprite)
        return;

    auto pos = sprite->getPosition();
    pos.y += body->getVelocity().y * dt;
    sprite->setPosition(pos);

    body->setOnGround(false);

    for (const auto& col : colliders)
    {
        auto rectY = body->getPhysicsRect();

        if (!rectY.intersectsRect(col.rect))
            continue;

        if (col.type == ColliderType::OneWay)
        {
            if (body->getVelocity().y >= 0)
                continue;

            float prevBottom  = rectY.getMinY() - body->getVelocity().y * dt;
            float platformTop = col.rect.getMaxY();

            if (prevBottom < platformTop)
                continue;

            float penetration = platformTop - rectY.getMinY();
            pos.y += penetration;

            sprite->setPosition(pos);
            body->setVelocityY(0);
            body->setOnGround(true);
        }
        else if (col.type == ColliderType::Solid)
        {
            if (body->getVelocity().y < 0)
            {
                float penetration = col.rect.getMaxY() - rectY.getMinY();
                pos.y += penetration;

                sprite->setPosition(pos);
                body->setVelocityY(0);
                body->setOnGround(true);
            }
        }
    }
}

void PhysicsSystem::updatePhysics(PhysicsEntity* body, float dt)
{
    moveAndCollideX(body, dt);
    moveAndCollideY(body, dt);
}
