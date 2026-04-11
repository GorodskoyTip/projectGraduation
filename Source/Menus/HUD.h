#pragma once
#include "axmol.h"

class HUD : public ax::Node
{
public:
    CREATE_FUNC(HUD);

    virtual bool init() override;

    void setPlayerHP(float current, float max);
    void setBossHP(float current, float max);
    void showBossBar(bool show);

private:
    ax::Sprite* playerBg   = nullptr;
    ax::Sprite* playerFill = nullptr;

    ax::Sprite* bossBg   = nullptr;
    ax::Sprite* bossFill = nullptr;
};
