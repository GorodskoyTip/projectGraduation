#pragma once
#include "axmol.h"

class MainMenu : public ax::Scene
{
public:
    static ax::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(MainMenu);

private:
    void onNewGame(ax::Object* sender);
    void onContinue(ax::Object* sender);
    void onSettings(ax::Object* sender);
    void onExit(ax::Object* sender);
};
