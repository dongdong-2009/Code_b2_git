#pragma once
#include "TVSSMethod.h"
#include "TVSSProtocol.h"

namespace TA_IRS_App
{

class TVSSSequencingScreenMethod :public TVSSMethod
{
public:
	TVSSSequencingScreenMethod(unsigned char monitorId, unsigned char sequenceId);
public:
	virtual ~TVSSSequencingScreenMethod(void);
};
};