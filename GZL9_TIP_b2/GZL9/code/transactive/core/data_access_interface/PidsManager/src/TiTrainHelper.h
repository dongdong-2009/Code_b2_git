#pragma once;

#include "TiTrain.h"
#include <vector>

class TiTrainHelper
{
    public:
        TiTrain load(unsigned long key);
        std::vector<TiTrain> load();
};