#include "MainMenu.h"
#include "Scenes/GameScene.h"

USING_NS_AX;

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

bool MainMenu::init()
{
    if (!Scene::init())
        return false;

    auto visibleSize = _director->getVisibleSize();
    Vec2 origin      = _director->getVisibleOrigin();

    auto title = Label::createWithTTF("MY GAME", "fonts/m6x11plus.ttf", 48);
    title->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height - 100));
    addChild(title);

    auto newGameItem  = MenuItemLabel::create(Label::createWithTTF("New Game", "fonts/m6x11plus.ttf", 32),
                                              AX_CALLBACK_1(MainMenu::onNewGame, this));
    auto continueItem = MenuItemLabel::create(Label::createWithTTF("Continue", "fonts/m6x11plus.ttf", 32),
                                              AX_CALLBACK_1(MainMenu::onContinue, this));
    auto settingsItem = MenuItemLabel::create(Label::createWithTTF("Settings", "fonts/m6x11plus.ttf", 32),
                                              AX_CALLBACK_1(MainMenu::onSettings, this));
    auto exitItem     = MenuItemLabel::create(Label::createWithTTF("Quit Game", "fonts/m6x11plus.ttf", 32),
                                              AX_CALLBACK_1(MainMenu::onExit, this));

    auto menu = Menu::create(newGameItem, continueItem, settingsItem, exitItem, nullptr);
    menu->alignItemsVerticallyWithPadding(20);
    menu->setPosition(origin + visibleSize / 2);

    addChild(menu);

    return true;
}

void MainMenu::onNewGame(Object* sender)
{
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(scene);
}

void MainMenu::onContinue(Object* sender)
{
    // TODO: Save on exit and load game
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(scene);
}

void MainMenu::onSettings(Object* sender)
{
    AXLOG("Settings Clicked");
}

void MainMenu::onExit(Object* sender)
{
    Director::getInstance()->end();
}
