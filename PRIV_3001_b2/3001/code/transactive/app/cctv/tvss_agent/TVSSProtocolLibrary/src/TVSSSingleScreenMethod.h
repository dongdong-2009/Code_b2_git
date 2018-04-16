#pragma once
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

namespace TA_IRS_App
{

class TVSSSingleScreenMethod :public TVSSMethod
{
public:
	TVSSSingleScreenMethod(unsigned char trainID,unsigned char monitorId, unsigned char cameraX);
public:
	virtual ~TVSSSingleScreenMethod(void);
};
};