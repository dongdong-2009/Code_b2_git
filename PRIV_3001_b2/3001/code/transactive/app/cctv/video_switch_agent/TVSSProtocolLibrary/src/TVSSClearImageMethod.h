#pragma once
#include "TVSSMethod.h"

namespace TA_IRS_App
{

class TVSSClearImageMethod :public TVSSMethod 
{
public:
	TVSSClearImageMethod(unsigned char monitorId );
public:
	virtual ~TVSSClearImageMethod(void);
};
};
