#include "HUD.h"

bool HUD::init()
{
    if (!Node::init())
        return false;

    auto visibleSize = ax::Director::getInstance()->getVisibleSize();

    // ================= PLAYER =================
    playerBg = ax::Sprite::create("GUI/HUD/ValueBar.png");
    playerBg->setAnchorPoint(ax::Vec2(0, 1));  // левый верх
    playerBg->setPosition(20, visibleSize.height - 20);
    addChild(playerBg);

    playerFill = ax::Sprite::create("GUI/HUD/ValueRed.png");
    playerFill->setAnchorPoint(ax::Vec2(0, 1));  // ВАЖНО
    playerFill->setPosition(ax::Vec2(4, 12));
    playerBg->addChild(playerFill);

    // ================= BOSS =================
    bossBg = ax::Sprite::create("GUI/HUD/ValueBarBig.png");
    bossBg->setAnchorPoint(ax::Vec2(0.5f, 0));
    bossBg->setPosition(visibleSize.width / 2, 30);
    addChild(bossBg);

    bossFill = ax::Sprite::create("GUI/HUD/ValueRedBig.png");
    bossFill->setAnchorPoint(ax::Vec2(0, 0));
    bossFill->setPosition(ax::Vec2(4, 4));
    bossBg->addChild(bossFill);

    bossBg->setVisible(false);
    bossFill->setVisible(false);

    return true;
}

void HUD::setPlayerHP(float current, float max)
{
    float percent = current / max;
    percent       = ax::clampf(percent, 0.f, 1.f);

    playerFill->setScaleX(percent);
}

void HUD::setBossHP(float current, float max)
{
    float percent = current / max;
    percent       = ax::clampf(percent, 0.0f, 1.0f);

    bossFill->setScaleX(percent);
}

void HUD::showBossBar(bool show)
{
    bossBg->setVisible(show);
    bossFill->setVisible(show);
}
