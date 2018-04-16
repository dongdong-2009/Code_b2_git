#include "StdAfx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerUtility.h"

namespace TA_IRS_App
{
	TunnelModeManagerUtility::TunnelModeManagerUtility(void)
	{
	}

	TunnelModeManagerUtility::~TunnelModeManagerUtility(void)
	{
	}

	unsigned long TunnelModeManagerUtility::stringToUnsignedLong(const std::string& value)
	{
		unsigned long returnValue;
		std::stringstream ss;
		ss << value;
		ss >> returnValue;
		return returnValue;
	}

}
