#pragma once

#include <string>
#include <vector>


struct Parameter
{
    std::string Key;
    int Type;
};

typedef std::vector<Parameter> Parameters;

struct ActivityItem
{
    bool Visable;
    unsigned int ID;
    std::string Name;
    std::string Resource;
};

typedef std::vector<ActivityItem> ActivityItems;
