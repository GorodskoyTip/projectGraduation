#include "TMXExporter.h"
#include <fstream>
#include <sstream>
#include <iostream>

void TMXExporter::save(const std::vector<std::vector<int>>& tiles, const std::string& filename)
{
    int height = tiles.size();
    int width  = tiles[0].size();

    // 1. Читаем template.tmx
    std::ifstream in("D:/GameDev/projectGraduation/Content/Level/template.tmx");
    if (!in.is_open())
        std::cout << "FAILED TO OPEN TEMPLATE\n";
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    // 2. Генерируем CSV
    std::stringstream csv;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            csv << tiles[y][x];

            // запятая после каждого элемента, кроме последнего во всей карте
            if (!(y == height - 1 && x == width - 1))
                csv << ",";
        }

        if (y != height - 1)
            csv << "\n";
    }

    // 3. Заменяем <data>...</data>
    size_t start = content.find("<data");
    size_t end   = content.find("</data>");

    if (start == std::string::npos || end == std::string::npos)
        return;

    size_t dataStart = content.find(">", start) + 1;

    content.replace(dataStart, end - dataStart, "\n" + csv.str() + "\n");

    std::cout << "Width: " << width << ", Height: " << height << std::endl;

    // 4. Сохраняем
    std::ofstream out(filename);
    out << content;
    out.close();
}
