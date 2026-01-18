#include "GameScene.h"
#include "Entities/Player.h"
#include "Core/LevelBounds.h"

USING_NS_AX;

constexpr float DEAD_ZONE_X = 200.0f;
constexpr float CAMERA_FOLLOW_SPEED = 6.0f;
constexpr float LOOK_AHEAD = 100.0f;

GameScene* GameScene::create()
{
    auto scene = new (std::nothrow) GameScene();
    if (scene && scene->init())
    {
        scene->autorelease();
        return scene;
    }
    AX_SAFE_DELETE(scene);
    return nullptr;
}

bool GameScene::init()
{
    if (!Scene::init())
        return false;

    world = Node::create();
    this->addChild(world);

    colliders.emplace_back(ax::Rect(0, 0, 3000, 100), ColliderType::Solid); //Ground
    auto ground = ax::LayerColor::create(ax::Color4B::RED, 3000, 100);
    ground->setPosition(0, 0);
    world->addChild(ground);

    colliders.emplace_back(ax::Rect(0, 0, 50, 1000), ColliderType::Solid); //Left Wall
    auto wallL = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallL->setPosition(0, 0);
    world->addChild(wallL);

    colliders.emplace_back(ax::Rect(2950, 0, 50, 1000), ColliderType::Solid); //Right Wall
    auto wallR = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallR->setPosition(2950, 0);
    world->addChild(wallR);

    colliders.emplace_back(ax::Rect(600, 150, 200, 20), ColliderType::OneWay);
    auto platform = ax::LayerColor::create(ax::Color4B::GREEN, 150, 20);
    platform->setPosition(600, 150);
    world->addChild(platform);


    player = Player::create();
    player->setPosition(200, 240);
    world->addChild(player);

    scheduleUpdate();
    return true;
}

void GameScene::updateCamera(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float screenCenterX = visibleSize.width / 2;
    float playerScreenX = player->getPositionX() + world->getPositionX();

    float leftDead  = screenCenterX - DEAD_ZONE_X;
    float rightDead = screenCenterX + DEAD_ZONE_X;

    float targetWorldX = world->getPositionX() - player->getDirection() * LOOK_AHEAD;

    if (playerScreenX < leftDead)
        targetWorldX += leftDead - playerScreenX;
    else if (playerScreenX > rightDead)
        targetWorldX -= playerScreenX - rightDead;

    float newX = world->getPositionX() +
        (targetWorldX - world->getPositionX()) * CAMERA_FOLLOW_SPEED * dt;

    float minX = visibleSize.width - LEVEL_RIGHT;
    float maxX = -LEVEL_LEFT;

    newX = std::clamp(newX, minX, maxX);
    world->setPositionX(newX);
}

void GameScene::update(float dt)
{
    if (!world || !player)
        return;

    player->update(dt);

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

    pos = player->getPosition();
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

    updateCamera(dt);
}
