#pragma once
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

namespace TA_IRS_App
{

	class TVSSAcknowledgeMethod :public TVSSMethod
	{
	public:
		TVSSAcknowledgeMethod(unsigned char trainID,unsigned char monitorId ,unsigned char cameraX,unsigned char cameraY);
	public:
		virtual ~TVSSAcknowledgeMethod(void);
	};
};