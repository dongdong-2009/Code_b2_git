#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerUtility)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerUtility
#if _MSC_VER > 1000
#pragma once
#endif
#include "app/bas/TimeTableManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	class TimeTableManagerUtility
	{
	public:
		TimeTableManagerUtility(void);
		~TimeTableManagerUtility(void);
	public:
		static unsigned long stringToUnsignedLong(const std::string& value);
		static std::string unsignedLongToString(unsigned long value);
	};
}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerUtility