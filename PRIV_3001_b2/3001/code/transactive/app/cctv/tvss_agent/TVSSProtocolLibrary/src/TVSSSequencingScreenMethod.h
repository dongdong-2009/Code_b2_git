#pragma once
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

namespace TA_IRS_App
{

class TVSSSequencingScreenMethod :public TVSSMethod
{
public:
	TVSSSequencingScreenMethod(unsigned char trainID,unsigned char monitorId, unsigned char sequenceId);
public:
	virtual ~TVSSSequencingScreenMethod(void);
};
};