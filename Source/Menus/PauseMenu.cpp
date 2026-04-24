#include "PauseMenu.h"
#include "MainMenu.h"

USING_NS_AX;

bool PauseMenu::init()
{
    if (!LayerColor::initWithColor(Color4B(0, 0, 0, 180)))
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto resumeItem   = MenuItemSprite::create(Sprite::create("GUI/MainMenu/ResumeButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(PauseMenu::onResume, this));
    auto saveItem     = MenuItemSprite::create(Sprite::create("GUI/MainMenu/SaveButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(PauseMenu::onSave, this));
    auto loadItem     = MenuItemSprite::create(Sprite::create("GUI/MainMenu/LoadButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(PauseMenu::onLoad, this));
    auto settingsItem = MenuItemSprite::create(Sprite::create("GUI/MainMenu/SettingsButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(PauseMenu::onSettings, this));
    auto mainMenuItem = MenuItemSprite::create(Sprite::create("GUI/MainMenu/MainMenuButton.png"),
                                               Sprite::create("GUI/MainMenu/HighlightButtonBig.png"),
                                               AX_CALLBACK_1(PauseMenu::onMainMenu, this));

    auto menu = Menu::create(resumeItem, saveItem, loadItem, settingsItem, mainMenuItem, nullptr);
    menu->alignItemsVerticallyWithPadding(20);
    menu->setPosition(visibleSize / 2);

    addChild(menu);

    return true;
}

void PauseMenu::setGameScene(Scene* scene)
{
    gameScene = scene;
}

void PauseMenu::onResume(Object* sender)
{
    Director::getInstance()->resume();
    this->removeFromParent();
}

void PauseMenu::onSave(Object* sender)
{
    AXLOG("Game Saved");
}

void PauseMenu::onLoad(Object* sender)
{
    AXLOG("Game Loaded");
}

void PauseMenu::onSettings(Object* sender)
{
    AXLOG("Settings");
}

void PauseMenu::onMainMenu(Object* sender)
{
    Director::getInstance()->resume();
    this->removeFromParent();
    auto scene = MainMenu::createScene();
    Director::getInstance()->replaceScene(scene);
}
