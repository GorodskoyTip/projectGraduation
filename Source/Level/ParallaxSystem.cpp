#include "ParallaxSystem.h"

void ParallaxSystem::addLayer(const std::string& texture, float factor, int z, Node* parent)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto temp = Sprite::create(texture);

    float scale = visibleSize.height / temp->getContentSize().height;
    float width = temp->getContentSize().width * scale;

    int count = ceil(visibleSize.width / width) + 2;

    ParallaxLayer layer;
    layer.factor = factor;
    layer.width  = width;

    for (int i = 0; i < count; i++)
    {
        auto tile = Sprite::create(texture);
        tile->setAnchorPoint(Vec2(0, 0));
        tile->setScale(scale);
        tile->setPosition(i * width, 0);

        parent->addChild(tile, z);
        layer.tiles.push_back(tile);
    }

    layers.push_back(layer);
}

void ParallaxSystem::update(float cameraX)
{
    for (auto layer : layers)
    {
        float offset = fmod(cameraX * layer.factor, layer.width);

        for (int i = 0; i < layer.tiles.size(); i++)
        {
            float x = i * layer.width - offset;
            layer.tiles[i]->setPosition(x, 0);
        }
    }
}
