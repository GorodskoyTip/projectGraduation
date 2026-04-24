#pragma once
#include <vector>
#include <string>

class TMXExporter
{
public:
    static void save(const std::vector<std::vector<int>>& tiles, const std::string& filename);
};
