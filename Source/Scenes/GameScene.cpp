#include "GameScene.h"
#include "Entities/Player.h"
#include "Core/LevelBounds.h"

USING_NS_AX;

constexpr float DEAD_ZONE_X = 200.0f;
constexpr float CAMERA_FOLLOW_SPEED = 8.0f;
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
    AXLOG("GAME SCENE INIT");

    if (!Scene::init())
        return false;

    world = Node::create();
    this->addChild(world);

    player = Player::create();
    player->setPosition(0, 240);
    world->addChild(player);


    auto box = ax::LayerColor::create(ax::Color4B::RED, 50, 50);
    box->setPosition(200, 240);
    world->addChild(box);


    scheduleUpdate();
    return true;
}

void GameScene::update(float dt)
{
    if (!world || !player)
        return;

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
