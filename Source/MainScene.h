#pragma once
#include "axmol.h"

class MainScene : public ax::Scene
{
public:
    static MainScene* create();
    virtual bool init() override;
    virtual void onEnter() override;
};
