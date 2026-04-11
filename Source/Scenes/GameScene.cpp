#include "GameScene.h"
#include "Entities/Player.h"
#include "Entities/Canine.h"
#include "Entities/Werewolf.h"
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

    hud = HUD::create();
    hud->setPosition(Vec2::ZERO);
    this->addChild(hud, 1000);

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
    auto platform = ax::LayerColor::create(ax::Color4B::BLUE, 150, 20);
    platform->setPosition(600, 150);
    world->addChild(platform);

    physics.addCollider({ax::Rect(1000, 150, 150, 20), ColliderType::OneWay});
    auto platform1 = ax::LayerColor::create(ax::Color4B::BLUE, 150, 20);
    platform1->setPosition(1000, 150);
    world->addChild(platform1);

    player = Player::create();
    player->setPosition(200, 240);
    world->addChild(player);

    //spawnPoints.push_back({EnemyType::Canine, {400, 240}});
    //spawnPoints.push_back({EnemyType::Canine, {700, 240}});

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

    bossArena = ax::Rect(1000, 0, 800, 400);
    boss      = Werewolf::create();
    boss->setPosition(1800, 200);
    boss->setTarget(player);
    boss->setArena(bossArena);
    world->addChild(boss);

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
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_F4)
        {
            freeCamera = !freeCamera;

            // при включении — запоминаем текущую позицию
            if (freeCamera)
                freeCamPos = world->getPosition();

            AXLOG("Free camera: %d", freeCamera);
        }
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_W)
            camUp = true;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_S)
            camDown = true;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_A)
            camLeft = true;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_D)
            camRight = true;
    };

    listener->onKeyReleased = [this](ax::EventKeyboard::KeyCode keyCode, ax::Event*) {
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_W)
            camUp = false;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_S)
            camDown = false;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_A)
            camLeft = false;
        if (keyCode == ax::EventKeyboard::KeyCode::KEY_D)
            camRight = false;
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

            auto pos = player->getPosition();
            debugDraw->drawDot(pos, 5.0f, ax::Color4F(1, 0, 1, 1));
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

        if (boss)
        {
            auto bossRect     = boss->getPhysicsRect();
            ax::Vec2 verts[4] = {{bossRect.getMinX(), bossRect.getMinY()},
                                 {bossRect.getMaxX(), bossRect.getMinY()},
                                 {bossRect.getMaxX(), bossRect.getMaxY()},
                                 {bossRect.getMinX(), bossRect.getMaxY()}};

            debugDraw->drawPoly(verts, 4, true, ax::Color4F(0, 0, 1, 1));  // ЖЁЛТЫЙ

            auto pos = boss->getPosition();
            debugDraw->drawDot(pos, 5.0f, ax::Color4F(1, 0, 1, 1));  // ФИОЛЕТОВАЯ
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

        if (boss && boss->isAttackActive())
        {
            auto hit = boss->getHitBox();

            ax::Vec2 verts[4] = {{hit.getMinX(), hit.getMinY()},
                                 {hit.getMaxX(), hit.getMinY()},
                                 {hit.getMaxX(), hit.getMaxY()},
                                 {hit.getMinX(), hit.getMaxY()}};

            debugDraw->drawPoly(verts, 4, true, ax::Color4F(0, 1, 1, 1));  // ЗЕЛЁНЫЙ
        }
    }
}

void GameScene::updateFreeCamera(float dt)
{
    if (!freeCamera)
        return;

    ax::Vec2 dir = ax::Vec2::ZERO;

    if (camUp)
        dir.y += 1;
    if (camDown)
        dir.y -= 1;
    if (camLeft)
        dir.x -= 1;
    if (camRight)
        dir.x += 1;

    if (dir != ax::Vec2::ZERO)
    {
        dir.normalize();
        freeCamPos += dir * freeCamSpeed * dt;
    }

    world->setPosition(freeCamPos);
}

void GameScene::updatePlayerAttack(float dt)
{
    for (auto enemy : enemies)
    {
        if (!enemy || enemy->isDead())
            continue;

        if (!player->isAttackActive())
            continue;

        if (player->getHitBox().intersectsRect(enemy->getHurtBox()))
            enemy->receiveDamage(player->getAttackDamage(player->getAttackType()), player->getAttackID());
    }
    if (boss && !boss->isDead())
    {
        if (player->isAttackActive())
        {
            if (player->getHitBox().intersectsRect(boss->getPhysicsRect()))
            {
                boss->receiveDamage(player->getAttackDamage(player->getAttackType()), player->getAttackID());
            }
        }
    }
    if (boss && boss->isDead())
    {
        exitingBossFight = true;
        cameraLockActive = false;
    }
}

void GameScene::updateEnemyAttack(float dt)
{
    for (auto enemy : enemies)
    {
        if (!enemy || enemy->isDead())
            continue;

        if (!enemy->isAttackActive())
            continue;

        if (enemy->getHitBox().intersectsRect(player->getHurtBox()))
            player->receiveDamage(enemy->getAttackDamage());
    }
}

void GameScene::updateBossAttack(float dt)
{
    if (!boss || boss->isDead())
        return;

    if (!boss->isAttackActive())
        return;

    if (boss->getHitBox().intersectsRect(player->getHurtBox()))
    {
        player->receiveDamage(boss->getAttackDamage());
    }
}

bool GameScene::isBossFightTriggered() const
{
    ax::Rect bossTriggerZone = ax::Rect(bossArena.getMidX() - 100, bossArena.getMinY(), 200, bossArena.size.height);
    return bossTriggerZone.containsPoint(player->getPosition());
}

void GameScene::startBossFight()
{
    arenaLocked      = true;
    bossFightStarted = true;

    float wallWidth = 20.f;

    Collider leftWall  = {ax::Rect(bossArena.getMinX() - wallWidth, 0, wallWidth, 1000), ColliderType::Solid};
    Collider rightWall = {ax::Rect(bossArena.getMaxX(), 0, wallWidth, 1000), ColliderType::Solid};

    physics.addCollider(leftWall);
    physics.addCollider(rightWall);

    arenaColliders.push_back(leftWall);
    arenaColliders.push_back(rightWall);

    auto visibleSize   = ax::Director::getInstance()->getVisibleSize();
    float arenaCenterX = bossArena.getMidX();

    cameraTargetX = visibleSize.width * 0.5f - arenaCenterX;
}

void GameScene::endBossFight()
{
    arenaLocked      = false;
    bossFightStarted = false;
    cameraLockActive = false;

    for (auto& col : arenaColliders)
    {
        physics.removeCollider(col);
    }

    arenaColliders.clear();

    exitingBossFight = true;
}

void GameScene::updateCamera(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float currentX = world->getPositionX();
    float targetX  = currentX;

    // 🔥 1. Режим выхода из босса
    if (exitingBossFight)
    {
        float playerWorldX = player->getPositionX();
        targetX            = visibleSize.width * 0.5f - playerWorldX;

        float newX = currentX + (targetX - currentX) * 3.0f * dt;

        // ✅ clamp применяем
        float minX = visibleSize.width - LEVEL_RIGHT;
        float maxX = -LEVEL_LEFT;
        newX       = std::clamp(newX, minX, maxX);

        world->setPositionX(newX);

        if (std::abs(newX - targetX) < 1.0f)
        {
            exitingBossFight = false;
        }

        return;
    }

    // 🔥 2. Боссфайт (движение к арене)
    if (bossFightStarted && !cameraLockActive)
    {
        targetX = cameraTargetX;

        float newX = currentX + (targetX - currentX) * 5.0f * dt;
        world->setPositionX(newX);

        if (std::abs(newX - cameraTargetX) < 1.0f)
        {
            cameraLockActive = true;
            world->setPositionX(cameraTargetX);
        }

        return;
    }

    // 🔥 3. Зафиксированная камера
    if (bossFightStarted && cameraLockActive)
    {
        world->setPositionX(cameraTargetX);
        return;
    }

    // 🔥 4. Обычная камера (ПОСЛЕ босса тоже сюда попадём)
    float playerWorldX = player->getPositionX();
    targetX            = visibleSize.width * 0.5f - playerWorldX;

    float minX = visibleSize.width - LEVEL_RIGHT;
    float maxX = -LEVEL_LEFT;

    targetX = std::clamp(targetX, minX, maxX);

    float newX = currentX + (targetX - currentX) * 10.0f * dt;
    world->setPositionX(newX);
}

void GameScene::update(float dt)
{
    if (!world || !player)
        return;

    hud->setPlayerHP(player->getHP(), 100);

    if (boss && bossFightStarted)
    {
        hud->showBossBar(true);
        hud->setBossHP(boss->getHP(), 300);
    }
    else
    {
        hud->showBossBar(false);
    }

    if (freeCamera)
    {
        updateFreeCamera(dt);
        drawDebug();
        return;  // 🔥 отключаем всё остальное
    }

    player->update(dt);
    physics.updatePhysics(player, dt);

    for (auto enemy : enemies)
    {
        enemy->update(dt);
        physics.updatePhysics(enemy, dt);
    }

    if (boss)
    {
        boss->update(dt);
        physics.updatePhysics(boss, dt);
    }

    updatePlayerAttack(dt);
    updateEnemyAttack(dt);
    updateBossAttack(dt);

    if (boss && !arenaLocked && isBossFightTriggered())
        startBossFight();
    if (boss && boss->isDead() && arenaLocked)
        endBossFight();

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

    if (boss && boss->readyToRemove())
    {
        boss->removeFromParent();
        boss = nullptr;
    }

    updateCamera(dt);

    drawDebug();
}
