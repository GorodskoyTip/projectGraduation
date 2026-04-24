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

    auto background = Sprite::create("GUI/MainMenu/Background.png");
    background->setPosition(origin + visibleSize / 2);
    background->setScale(visibleSize.width / background->getContentSize().width,
                         visibleSize.height / background->getContentSize().height);
    addChild(background, -1);

    auto title = Label::createWithTTF("The Good Old Tale\nof Red Riding Hood", "fonts/m6x11plus.ttf", 48);
    title->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height - 100));
    addChild(title);

    auto newGameItem  = MenuItemSprite::create(Sprite::create("GUI/MainMenu/NewGameButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(MainMenu::onNewGame, this));
    auto continueItem = MenuItemSprite::create(Sprite::create("GUI/MainMenu/ContinueButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(MainMenu::onContinue, this));
    auto settingsItem = MenuItemSprite::create(Sprite::create("GUI/MainMenu/SettingsButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(MainMenu::onSettings, this));
    auto exitItem     = MenuItemSprite::create(Sprite::create("GUI/MainMenu/QuitGameButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(MainMenu::onExit, this));

    newGameItem->setCallback([this](ax::Object* sender) {
        auto item = static_cast<MenuItemSprite*>(sender);
        item->runAction(Sequence::create(ScaleTo::create(0.1f, 1.1f), ScaleTo::create(0.1f, 1.0f), nullptr));
        onNewGame(sender);
    });

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
