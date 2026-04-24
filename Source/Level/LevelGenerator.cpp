#include "LevelGenerator.h"
#include <cstdlib>
#include <algorithm>

std::vector<std::vector<int>> LevelGenerator::generate(int width, int height)
{
    std::vector<std::vector<int>> tiles(height, std::vector<int>(width, 0));

    const int GROUND_ID = 12;

    int groundY = 5;

    for (int x = 0; x < width; x++)
    {
        groundY += rand() % 3 - 1;
        groundY = std::clamp(groundY, 3, 10);

        for (int y = height - 1; y >= height - 1 - groundY; y--)
            tiles[y][x] = GROUND_ID;
    }
    return tiles;
}
