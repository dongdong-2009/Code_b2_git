#if !defined(Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerUtility)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerUtility
#pragma once

#include <sstream>
namespace TA_IRS_App
{
	class TunnelModeManagerUtility
	{
	public:
		TunnelModeManagerUtility(void);
		~TunnelModeManagerUtility(void);
	public:
		static unsigned long stringToUnsignedLong(const std::string& value);

	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerUtility