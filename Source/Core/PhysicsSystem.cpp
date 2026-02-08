#include "PhysicsSystem.h"
#include "Entities/Player.h"

void PhysicsSystem::addCollider(const Collider& col) { colliders.push_back(col); }

void PhysicsSystem::moveAndCollideX(Player* player, float dt)
{
    auto pos = player->getPosition();
    pos.x += player->velocity.x * dt;
    player->setPosition(pos);

    auto rectX = player->getPhysicsRect();

    for (const auto& col : colliders)
    {
        if (col.rect.size.height < 200)
            continue;

        if (!rectX.intersectsRect(col.rect))
            continue;

        if (player->velocity.x > 0)
        {
            float penetration = rectX.getMaxX() - col.rect.getMinX();
            pos.x -= penetration;
        }
        else if (player->velocity.x < 0)
        {
            float penetration = col.rect.getMaxX() - rectX.getMinX();
            pos.x += penetration;
        }

        player->setPosition(pos);
        player->velocity.x = 0;
        break;
    }
}

void PhysicsSystem::moveAndCollideY(Player* player, float dt)
{
    auto pos = player->getPosition();
    pos.y += player->velocity.y * dt;
    player->setPosition(pos);

    player->setOnGround(false);

    auto rectY = player->getPhysicsRect();

    for (const auto& col : colliders)
    {
        if (!rectY.intersectsRect(col.rect))
            continue;

        if (col.type == ColliderType::OneWay)
        {
            if (player->velocity.y >= 0)
                continue;

            float prevBottom = rectY.getMinY() - player->velocity.y * dt;
            float platformTop = col.rect.getMaxY();

            if (prevBottom < platformTop)
                continue;

            float penetration = platformTop - rectY.getMinY();
            pos.y += penetration;

            player->setPosition(pos);
            player->velocity.y = 0;
            player->setOnGround(true);
        }

        else if (col.type == ColliderType::Solid)
        {
            if (player->velocity.y < 0)
            {
                float penetration = col.rect.getMaxY() - rectY.getMinY();
                pos.y += penetration;

                player->setPosition(pos);
                player->velocity.y = 0;
                player->setOnGround(true);
            }
        }
    }
}

void PhysicsSystem::update(Player* player, float dt)
{
    moveAndCollideX(player, dt);
    moveAndCollideY(player, dt);
}
