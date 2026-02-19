#include "PhysicsSystem.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"

void PhysicsSystem::addCollider(const Collider& col) { colliders.push_back(col); }

void PhysicsSystem::moveAndCollideX(Player* player, float dt)
{
    auto pos = player->getPosition();
    pos.x += player->getVelocity().x * dt;
    player->setPosition(pos);


    for (const auto& col : colliders)
    {
        auto rectX = player->getPhysicsRect();

        if (col.rect.size.height < 200)
            continue;

        if (!rectX.intersectsRect(col.rect))
            continue;

        if (player->getVelocity().x > 0)
        {
            float penetration = rectX.getMaxX() - col.rect.getMinX();
            pos.x -= penetration;
        }
        else if (player->getVelocity().x < 0)
        {
            float penetration = col.rect.getMaxX() - rectX.getMinX();
            pos.x += penetration;
        }

        player->setPosition(pos);
        player->setVelocityX(0);
        break;
    }
}

void PhysicsSystem::moveAndCollideY(Player* player, float dt)
{
    auto pos = player->getPosition();
    pos.y += player->getVelocity().y * dt;
    player->setPosition(pos);

    player->setOnGround(false);

    for (const auto& col : colliders)
    {
        auto rectY = player->getPhysicsRect();

        if (!rectY.intersectsRect(col.rect))
            continue;

        if (col.type == ColliderType::OneWay)
        {
            if (player->getVelocity().y >= 0)
                continue;

            float prevBottom  = rectY.getMinY() - player->getVelocity().y * dt;
            float platformTop = col.rect.getMaxY();

            if (prevBottom < platformTop)
                continue;

            float penetration = platformTop - rectY.getMinY();
            pos.y += penetration;

            player->setPosition(pos);
            player->setVelocityY(0);
            player->setOnGround(true);
        }
        else if (col.type == ColliderType::Solid)
        {
            if (player->getVelocity().y < 0)
            {
                float penetration = col.rect.getMaxY() - rectY.getMinY();
                pos.y += penetration;

                player->setPosition(pos);
                player->setVelocityY(0);
                player->setOnGround(true);
            }
        }
    }
}


void PhysicsSystem::updatePlayer(Player* player, float dt)
{
    moveAndCollideX(player, dt);
    moveAndCollideY(player, dt);
}

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
