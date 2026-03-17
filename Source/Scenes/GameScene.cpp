#include "GameScene.h"
#include "Entities/Player.h"
#include "Entities/Canine.h"
#include "Core/LevelBounds.h"

#include <iostream>

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
    auto ground = ax::LayerColor::create(ax::Color4B::RED, 3000, 100);
    ground->setPosition(0, 0);
    world->addChild(ground);

    physics.addCollider({ax::Rect(0, 0, 50, 1000), ColliderType::Solid});
    auto wallL = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallL->setPosition(0, 0);
    world->addChild(wallL);

    physics.addCollider({ax::Rect(2950, 0, 50, 1000), ColliderType::Solid});
    auto wallR = ax::LayerColor::create(ax::Color4B::GRAY, 50, 1000);
    wallR->setPosition(2950, 0);
    world->addChild(wallR);

    physics.addCollider({ax::Rect(600, 150, 150, 20), ColliderType::OneWay});
    auto platform = ax::LayerColor::create(ax::Color4B::GREEN, 150, 20);
    platform->setPosition(600, 150);
    world->addChild(platform);

    player = Player::create();
    player->setPosition(200, 240);
    world->addChild(player);

    spawnPoints.push_back({EnemyType::Canine, {400, 240}});
    spawnPoints.push_back({EnemyType::Canine, {700, 240}});

    for (auto& spawn : spawnPoints)
    {
        Enemy* enemy = createEnemy(spawn.type);

        if (!enemy)
            continue;

        enemy->setPosition(spawn.position);

        world->addChild(enemy);
        enemies.push_back(enemy);
    }

    for (auto enemy : enemies)
    {
        enemy->setTarget(player);
    }

    debugDraw = ax::DrawNode::create();
    world->addChild(debugDraw, 999);

    scheduleUpdate();
    return true;
}

void GameScene::onEnter()
{
    Scene::onEnter();

    auto listener = ax::EventListenerKeyboard::create();

    listener->onKeyPressed = [this](ax::EventKeyboard::KeyCode keyCode, ax::Event*) {
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_F1)
        {
            debugPhysics = !debugPhysics;
            AXLOG("Physics debug toggled: %d", debugPhysics);
        }
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_F2)
        {
            debugHurtBox = !debugHurtBox;
            AXLOG("Hurtbox debug toggled: %d", debugHurtBox);
        }
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_F3)
        {
            debugHitBox = !debugHitBox;
            AXLOG("Hitbox debug toggled: %d", debugHitBox);
        }
    };

    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
}

void GameScene::onExit()
{
    _eventDispatcher->removeEventListenersForTarget(this);
    Scene::onExit();
}

Enemy* GameScene::createEnemy(EnemyType type)
{
    switch (type)
    {
    case EnemyType::Canine:
        return Canine::create();
    default:
        return nullptr;
    }
}

void GameScene::drawDebug()
{
    debugDraw->clear();
    if (debugPhysics)
    {
        if (player)
        {
            auto playerPhysRect = player->getPhysicsRect();
            ax::Vec2 verts[4]   = {{playerPhysRect.getMinX(), playerPhysRect.getMinY()},
                                   {playerPhysRect.getMaxX(), playerPhysRect.getMinY()},
                                   {playerPhysRect.getMaxX(), playerPhysRect.getMaxY()},
                                   {playerPhysRect.getMinX(), playerPhysRect.getMaxY()}};
            debugDraw->drawPoly(verts, 4, true, ax::Color4F(0, 0, 1, 1));
        }

        for (auto enemy : enemies)
        {
            if (enemy)
            {
                auto enemyPhysRect = enemy->getPhysicsRect();
                ax::Vec2 everts[4] = {{enemyPhysRect.getMinX(), enemyPhysRect.getMinY()},
                                      {enemyPhysRect.getMaxX(), enemyPhysRect.getMinY()},
                                      {enemyPhysRect.getMaxX(), enemyPhysRect.getMaxY()},
                                      {enemyPhysRect.getMinX(), enemyPhysRect.getMaxY()}};
                debugDraw->drawPoly(everts, 4, true, ax::Color4F(0, 0, 1, 1));
            }
        }

        for (const auto& col : physics.getColliders())
        {
            auto c             = col.rect;
            ax::Vec2 cverts[4] = {{c.getMinX(), c.getMinY()},
                                  {c.getMaxX(), c.getMinY()},
                                  {c.getMaxX(), c.getMaxY()},
                                  {c.getMinX(), c.getMaxY()}};
            debugDraw->drawPoly(cverts, 4, true, ax::Color4F(0, 0, 1, 1));
        }
    }
    if (debugHurtBox)
    {
        if (player)
        {
            auto playerHurtBox = player->getHurtBox();
            ax::Vec2 verts[4]  = {{playerHurtBox.getMinX(), playerHurtBox.getMinY()},
                                  {playerHurtBox.getMaxX(), playerHurtBox.getMinY()},
                                  {playerHurtBox.getMaxX(), playerHurtBox.getMaxY()},
                                  {playerHurtBox.getMinX(), playerHurtBox.getMaxY()}};
            debugDraw->drawPoly(verts, 4, true, ax::Color4F(1, 0, 0, 1));
        }

        for (auto enemy : enemies)
        {
            if (enemy)
            {
                auto enemyHurtBox  = enemy->getHurtBox();
                ax::Vec2 everts[4] = {{enemyHurtBox.getMinX(), enemyHurtBox.getMinY()},
                                      {enemyHurtBox.getMaxX(), enemyHurtBox.getMinY()},
                                      {enemyHurtBox.getMaxX(), enemyHurtBox.getMaxY()},
                                      {enemyHurtBox.getMinX(), enemyHurtBox.getMaxY()}};
                debugDraw->drawPoly(everts, 4, true, ax::Color4F(1, 0, 0, 1));
            }
        }
    }
    if (debugHitBox)
    {
        if (player && player->isAttackActive())
        {
            auto playerHitBox = player->getHitBox();
            ax::Vec2 verts[4] = {{playerHitBox.getMinX(), playerHitBox.getMinY()},
                                 {playerHitBox.getMaxX(), playerHitBox.getMinY()},
                                 {playerHitBox.getMaxX(), playerHitBox.getMaxY()},
                                 {playerHitBox.getMinX(), playerHitBox.getMaxY()}};
            debugDraw->drawPoly(verts, 4, true, ax::Color4F(0, 1, 1, 1));
        }

        for (auto enemy : enemies)
        {
            if (enemy && enemy->isAttackActive())
            {
                auto enemyHitBox   = enemy->getHitBox();
                ax::Vec2 everts[4] = {{enemyHitBox.getMinX(), enemyHitBox.getMinY()},
                                      {enemyHitBox.getMaxX(), enemyHitBox.getMinY()},
                                      {enemyHitBox.getMaxX(), enemyHitBox.getMaxY()},
                                      {enemyHitBox.getMinX(), enemyHitBox.getMaxY()}};
                debugDraw->drawPoly(everts, 4, true, ax::Color4F(0, 1, 1, 1));
            }
        }
    }
}

void GameScene::updatePlayerAttack(float dt)
{
    for (auto enemy : enemies)
    {
        if (!enemy || enemy->isDead())
            return;

        if (!player->isAttackActive())
            return;

        if (player->getHitBox().intersectsRect(enemy->getHurtBox()))
            enemy->receiveDamage(player->getAttackDamage(player->getAttackType()), player->getAttackID());
    }
}

void GameScene::updateEnemyAttack(float dt)
{
    for (auto enemy : enemies)
    {
        if (!enemy || enemy->isDead())
            return;

        if (!enemy->isAttackActive())
            return;

        if (enemy->getHitBox().intersectsRect(player->getHurtBox()))
            player->receiveDamage(enemy->getAttackDamage());
    }
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

    for (auto enemy : enemies)
    {
        enemy->update(dt);
        physics.updateEnemy(enemy, dt);
    }

    updatePlayerAttack(dt);
    updateEnemyAttack(dt);

    for (auto it = enemies.begin(); it != enemies.end();)
    {
        if ((*it)->readyToRemove())
        {
            (*it)->removeFromParent();
            it = enemies.erase(it);
        }
        else
            ++it;
    }

    updateCamera(dt);

    drawDebug();
}
