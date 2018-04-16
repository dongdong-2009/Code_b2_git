#include "StdAfx.h"
#include "app/bas/BasParameterManager/src/BasParasManagerUtility.h"

namespace TA_IRS_App
{
	BasParasManagerUtility::BasParasManagerUtility(void)
	{
	}

	BasParasManagerUtility::~BasParasManagerUtility(void)
	{
	}

	unsigned long BasParasManagerUtility::stringToUnsignedLong(const std::string& value)
	{
		unsigned long retValue;
		std::stringstream ss;
		ss << value;	
		ss >> retValue;
		return retValue;
	}

	std::string BasParasManagerUtility::integerToString(int value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string BasParasManagerUtility::doubleToString(double value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}
