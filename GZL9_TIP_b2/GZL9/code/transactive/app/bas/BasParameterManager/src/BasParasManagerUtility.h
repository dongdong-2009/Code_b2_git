#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_BasParasManagerUtility)
#define Transactive_3002_GF_App_Bas_TimeTableManager_BasParasManagerUtility
#if _MSC_VER > 1000
#pragma once
#endif
#include "app/bas/BasParameterManager/src/CommonDefine.h"
namespace TA_IRS_App
{
	class BasParasManagerUtility
	{
	public:
		BasParasManagerUtility(void);
		~BasParasManagerUtility(void);
	public:
		static unsigned long stringToUnsignedLong(const std::string& value);
		static std::string integerToString(int value);
		static std::string doubleToString(double value);
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_BasParasManagerUtility