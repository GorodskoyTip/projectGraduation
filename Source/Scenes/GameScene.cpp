#include "GameScene.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Core/LevelBounds.h"

USING_NS_AX;

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

    physics.addCollider({ax::Rect(0, 0, 3000, 100), ColliderType::Solid});
    physics.addCollider({ax::Rect(0, 0, 50, 1000), ColliderType::Solid});
    physics.addCollider({ax::Rect(2950, 0, 50, 1000), ColliderType::Solid});
    physics.addCollider({ax::Rect(600, 150, 200, 20), ColliderType::OneWay});

    auto ground = ax::LayerColor::create(ax::Color4B::RED, 3000, 100);
    ground->setPosition(0, 0);
    world->addChild(ground);

    auto wallL = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallL->setPosition(0, 0);
    world->addChild(wallL);

    auto wallR = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallR->setPosition(2950, 0);
    world->addChild(wallR);

    auto platform = ax::LayerColor::create(ax::Color4B::GREEN, 150, 20);
    platform->setPosition(600, 150);
    world->addChild(platform);

    player = Player::create();
    player->setPosition(200, 240);
    world->addChild(player);

    enemy = Enemy::create();
    enemy->setPosition(1000, 540);
    world->addChild(enemy);

    scheduleUpdate();
    return true;
}

void GameScene::updateCamera(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float screenCenterX = visibleSize.width * 0.5f;
    float playerWorldX  = player->getPositionX();

    // 1. цель камеры — держать игрока в центре
    float targetWorldX = screenCenterX - playerWorldX;

    // 2. ограничение по уровню
    float minX = visibleSize.width - LEVEL_RIGHT; // правый край
    float maxX = -LEVEL_LEFT;                     // левый край

    targetWorldX = std::clamp(targetWorldX, minX, maxX);

    // 3. плавность (можно 1.0f для мгновенного следования)
    constexpr float CAMERA_SPEED = 10.0f;

    float currentX = world->getPositionX();
    float newX = currentX + (targetWorldX - currentX) * CAMERA_SPEED * dt;

    world->setPositionX(newX);
}

void GameScene::update(float dt)
{
    if (!world || !player)
        return;

    player->update(dt);
    physics.updatePlayer(player, dt);

    enemy->update(dt);
    physics.updateEnemy(enemy, dt);

    updateCamera(dt);
}
