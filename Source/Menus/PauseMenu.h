#pragma once
#include "axmol.h"

class PauseMenu : public ax::LayerColor
{
public:
    virtual bool init() override;

    CREATE_FUNC(PauseMenu);

    void setGameScene(ax::Scene* scene);

private:
    void onResume(ax::Object* sender);
    void onSave(ax::Object* sender);
    void onLoad(ax::Object* sender);
    void onSettings(ax::Object* sender);
    void onMainMenu(ax::Object* sender);

    ax::Scene* gameScene = nullptr;
};
