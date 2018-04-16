#pragma once
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"

namespace TA_IRS_App
{

class TVSSClearImageMethod :public TVSSMethod 
{
public:
	TVSSClearImageMethod(unsigned char trainID,unsigned char monitorId );
public:
	virtual ~TVSSClearImageMethod(void);

};
};
