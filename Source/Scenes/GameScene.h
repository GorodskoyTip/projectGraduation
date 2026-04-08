#pragma once

#include "axmol.h"
#include "Core/Collider.h"
#include "Core/PhysicsSystem.h"
#include "Entities/EnemyTypes.h"

#include <vector>

class Player;
class Enemy;
class Boss;

struct EnemySpawn
{
    EnemyType type;
    ax::Vec2 position;
};

class GameScene : public ax::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;
    virtual void update(float dt) override;
    void updateCamera(float dt);

    Enemy* createEnemy(EnemyType type);

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    virtual void onEnter() override;
    virtual void onExit() override;

    void drawDebug();
    void updateFreeCamera(float dt);

    void updatePlayerAttack(float dt);
    void updateEnemyAttack(float dt);
    void updateBossAttack(float dt);

    bool isBossFightTriggered() const;
    void startBossFight();
    void endBossFight();

private:
    PhysicsSystem physics;

    bool bossFightStarted = false;
    bool cameraLockActive = false;
    float cameraTargetX   = 0.f;

    Player* player          = nullptr;
    std::vector<EnemySpawn> spawnPoints;
    std::vector<Enemy*> enemies;
    Boss* boss              = nullptr;
    ax::Rect bossArena;
    bool arenaLocked        = false;
    Node* world             = nullptr;
    bool debugPhysics       = false;
    bool debugHurtBox       = false;
    bool debugHitBox        = false;
    ax::DrawNode* debugDraw = nullptr;

    bool freeCamera         = false;
    ax::Vec2 freeCamPos;
    float freeCamSpeed      = 600.0f;
    bool camUp         = false;
    bool camDown       = false;
    bool camLeft       = false;
    bool camRight      = false;

    std::vector<Collider> arenaColliders;
    bool exitingBossFight = false;
};
