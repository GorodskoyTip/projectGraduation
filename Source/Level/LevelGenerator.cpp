#include "LevelGenerator.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace
{
float random(int x)
{
    x = (x << 13) ^ x;
    return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float smooth(float t)
{
    return t * t * (3.f - 2.f * t);
}
}  // namespace

float LevelGenerator::perlin(float x)
{
    int x0 = (int)floor(x);
    int x1 = x0 + 1;

    float t = x - x0;

    float n0 = random(x0);
    float n1 = random(x1);

    return lerp(n0, n1, smooth(t));
}

std::vector<std::vector<int>> LevelGenerator::generate(int width, int height)
{
    std::vector<std::vector<int>> tiles(height, std::vector<int>(width, 0));

    const int GROUND = 1;

    float scale     = 0.08f;
    float amplitude = 6.f;
    int baseHeight  = height - 10;

    int prevGround = baseHeight;

    for (int x = 0; x < width; x++)
    {
        float n     = perlin(x * scale);
        int groundY = baseHeight + (int)(n * amplitude);

        groundY = std::clamp(groundY, prevGround - 3, prevGround + 3);
        groundY = std::clamp(groundY, height - 15, height - 3);

        prevGround = groundY;

        for (int y = height - 1; y >= groundY; y--)
        {
            tiles[y][x] = GROUND;
        }

        if (rand() % 20 == 0)
        {
            int gapWidth = rand() % 4 + 2;

            for (int gx = 0; gx < gapWidth && x + gx < width; gx++)
            {
                for (int y = 0; y < height; y++)
                    tiles[y][x + gx] = 0;
            }
            x += gapWidth;
            continue;
        }

        if (rand() % 12 == 0)
        {
            int py = groundY - (rand() % 4 + 3);
            int pw = rand() % 3 + 2;

            for (int px = 0; px < pw && x + px < width; px++)
                tiles[py][x + px] = GROUND;
        }
    }
    return tiles;
}
