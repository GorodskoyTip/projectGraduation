#include "GameScene.h"
#include "Entities/Player.h"
#include "Entities/Canine.h"
#include "Entities/Werewolf.h"
#include "Core/LevelBounds.h"

#include "Level/LevelGenerator.h"
#include "Level/TMXExporter.h"

void GameScene::generateLevelTest()
{
    auto tiles = LevelGenerator::generate(100, 40);
    TMXExporter::save(tiles, "D:/GameDev/projectGraduation/Content/Level/test.tmx");
}

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

ax::Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
        return false;

    // generateLevelTest();

    hud = HUD::create();
    hud->setPosition(Vec2::ZERO);
    this->addChild(hud, 1000);

    world = Node::create();
    this->addChild(world);

    auto map = TMXTiledMap::create("Level/test.tmx");
    if (!map)
    {
        AXLOG("FAILED TO LOAD TMX");
        return false;
    }
    world->addChild(map);

    auto layer = map->getLayer("Layer1");

    auto mapSize = map->getMapSize();
    AXLOG("Map: %d x %d", mapSize.width, mapSize.height);
    auto tileSize = map->getTileSize();
    AXLOG("Tile size: %f x %f", tileSize.width, tileSize.height);
    worldWidth  = mapSize.width * tileSize.width;
    worldHeight = mapSize.height * tileSize.height;

    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            int gid = layer->getTileGIDAt(ax::Vec2(x, y));

            if (gid == 0)
                continue;

            // 🔥 получаем свойства тайла
            auto props = map->getPropertiesForGID(gid);

            if (props.isNull())
                continue;

            // 🔥 проверяем solid
            if (props.getType() == ax::Value::Type::MAP)
            {
                auto& mapProps = props.asValueMap();

                if (mapProps.find("solid") != mapProps.end() && mapProps.at("solid").asBool())
                {
                    float worldX = x * tileSize.width;
                    float worldY = (mapSize.height - y - 1) * tileSize.height;

                    physics.addCollider(
                        {ax::Rect(worldX, worldY, tileSize.width, tileSize.height), ColliderType::Solid});
                }
            }
        }
    }

    player = Player::create();
    player->setPosition(200, 240);
    world->addChild(player);

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

        if (keyCode == ax::EventKeyboard::KeyCode::KEY_ESCAPE)
        {
            auto pause = this->getChildByName("PauseMenu");

            if (pause)
            {
                Director::getInstance()->resume();
                this->removeChildByName("PauseMenu");
            }
            else
            {
                auto pauseMenu = PauseMenu::create();
                pauseMenu->setName("PauseMenu");

                this->addChild(pauseMenu, 9999);
                Director::getInstance()->pause();
            }
        }
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

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
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

    // 🔥 Границы камеры из карты
    float minX = visibleSize.width - worldWidth;
    float maxX = 0.0f;

    // 🔥 1. Выход из босса
    if (exitingBossFight)
    {
        float playerWorldX = player->getPositionX();
        targetX            = visibleSize.width * 0.5f - playerWorldX;

        targetX = std::clamp(targetX, minX, maxX);

        float newX = currentX + (targetX - currentX) * 3.0f * dt;
        world->setPositionX(newX);

        if (std::abs(newX - targetX) < 1.0f)
        {
            exitingBossFight = false;
        }

        return;
    }

    // 🔥 2. Камера движется к арене босса
    if (bossFightStarted && !cameraLockActive)
    {
        float target = cameraTargetX;

        target = std::clamp(target, minX, maxX);

        float newX = currentX + (target - currentX) * 5.0f * dt;
        world->setPositionX(newX);

        if (std::abs(newX - target) < 1.0f)
        {
            cameraLockActive = true;
            world->setPositionX(target);
        }

        return;
    }

    // 🔥 3. Камера зафиксирована на арене
    if (bossFightStarted && cameraLockActive)
    {
        float clamped = std::clamp(cameraTargetX, minX, maxX);
        world->setPositionX(clamped);
        return;
    }

    // 🔥 4. Обычная камера (следует за игроком)
    float playerWorldX = player->getPositionX();
    targetX            = visibleSize.width * 0.5f - playerWorldX;

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
