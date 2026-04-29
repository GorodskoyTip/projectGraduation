#pragma once
#include "axmol.h"

USING_NS_AX;

struct ParallaxLayer
{
    std::vector<Sprite*> tiles;
    float factor;
    float width;
};

class ParallaxSystem
{
public:
    void addLayer(const std::string& texture, float factor, int z, Node* parent);
    void update(float cameraX);

private:
    std::vector<ParallaxLayer> layers;
};
