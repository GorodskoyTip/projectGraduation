#include "PhysicsSystem.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"

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

void PhysicsSystem::updatePlayer(Player* player, float dt)
{
    moveAndCollideX(player, dt);
    moveAndCollideY(player, dt);
}

void PhysicsSystem::updateEnemy(Enemy* enemy, float dt)
{
    auto pos = enemy->getPosition();
    pos.x += enemy->velocity.x * dt;
    enemy->setPosition(pos);

    auto rect = enemy->getPhysicsRect();

    for (const auto& col : colliders)
    {
        if (col.rect.size.height < 200)
            continue;
        if (!rect.intersectsRect(col.rect))
            continue;
        if (enemy->velocity.x > 0)
            pos.x -= rect.getMaxX() - col.rect.getMinX();
        else
            pos.x += col.rect.getMaxX() - rect.getMinX();

        enemy->setPosition(pos);
        enemy->changeDirection();
        enemy->velocity.x = 0;
        break;
    }

    pos = enemy->getPosition();
    pos.y += enemy->velocity.y * dt;
    enemy->setPosition(pos);

    enemy->setOnGround(false);

    rect = enemy->getPhysicsRect();

    for (const auto& col : colliders)
    {
        if (!rect.intersectsRect(col.rect))
            continue;
        if (enemy->velocity.y < 0)
        {
            pos.y += col.rect.getMaxY() - rect.getMinY();
            enemy->setPosition(pos);
            enemy->velocity.y = 0;
            enemy->setOnGround(true);
        }
    }
}
