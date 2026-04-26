#pragma once
#include <vector>

class LevelGenerator
{
public:
    static std::vector<std::vector<int>> generate(int width, int height);

private:
    static float perlin(float x);
};
