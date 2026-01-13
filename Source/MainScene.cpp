#include "MainScene.h"
#include "Scenes/GameScene.h"

USING_NS_AX;

bool MainScene::init()
{
    if (!Scene::init())
        return false;

    return true;
}

void MainScene::onEnter()
{
    Scene::onEnter();

    this->scheduleOnce([](float) {
        Director::getInstance()->replaceScene(GameScene::create());
    }, 0.0f, "go_to_game");
}

