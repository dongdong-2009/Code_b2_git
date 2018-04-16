#pragma once
#include "TVSSMethod.h"
#include "TVSSProtocol.h"

namespace TA_IRS_App
{

class TVSSSingleScreenMethod :public TVSSMethod
{
public:
	TVSSSingleScreenMethod(unsigned char monitorId, unsigned char cameraX);
public:
	virtual ~TVSSSingleScreenMethod(void);
};
};