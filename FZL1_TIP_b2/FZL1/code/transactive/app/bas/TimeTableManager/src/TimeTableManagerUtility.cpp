#include "StdAfx.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerUtility.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	TimeTableManagerUtility::TimeTableManagerUtility(void)
	{
	}

	TimeTableManagerUtility::~TimeTableManagerUtility(void)
	{
	}

	unsigned long TimeTableManagerUtility::stringToUnsignedLong(const std::string& value)
	{
		unsigned long retValue;
		std::stringstream ss;
		ss << value;	
		ss >> retValue;
		return retValue;
	}

	std::string TimeTableManagerUtility::unsignedLongToString(unsigned long value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}
