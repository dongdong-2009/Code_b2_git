#pragma once

#include "ViTriggeringMap.h"
#include <vector>

class ViTriggeringMapHelper
{
public:
	ViTriggeringMapHelper(void);
	~ViTriggeringMapHelper(void);
	ViTriggeringMap load(unsigned long triggeringEntity);
	std::vector<ViTriggeringMap> load();
};
