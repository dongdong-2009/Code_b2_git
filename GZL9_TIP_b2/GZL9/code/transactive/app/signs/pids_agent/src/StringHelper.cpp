/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/StringHelper.cpp $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  * A place holder for generic string functions
  *
  */


#include <iomanip>
#include <sstream>
#include <iostream>
#include <time.h>
#include "app/signs/pids_agent/src/StringHelper.h"
#include "core/alarm/src/AlarmConstants.h"

using namespace std;

const int StringHelper::DATE_TIME_LENGTH = 14;
const int StringHelper::YEAR_LENGTH   = 4;
const int StringHelper::MONTH_LENGTH  = 2;
const int StringHelper::DAY_LENGTH    = 2;
const int StringHelper::HOUR_LENGTH   = 2;
const int StringHelper::MINUTE_LENGTH = 2;
const int StringHelper::SECOND_LENGTH = 2;

const int StringHelper::YEAR_POS   = 0;
const int StringHelper::MONTH_POS  = 4;
const int StringHelper::DAY_POS    = 6;
const int StringHelper::HOUR_POS   = 8;
const int StringHelper::MINUTE_POS = 10;
const int StringHelper::SECOND_POS = 12;

bool StringHelper::isNumeric(const std::string& inString, long& outval)
{
	if ((inString.empty() == true) ||
		(inString.find_first_not_of("0123456789") != string::npos) )
	{
		return false;
	}

	outval = atol(inString.c_str());
	return true;
}

bool StringHelper::isNumeric(const std::string& inString, double& outval)
{
	if ((inString.empty() == true) ||
		(inString.find_first_not_of("0123456789.") != string::npos) ||
		(inString.find_first_of(".") != inString.find_last_of("."))) //If there is two dots in the string
	{
		return false;
	}

	outval = atof(inString.c_str());
	return true;
}

bool StringHelper::isValidTime(int year, int month, int day, int hour, int min, int sec, time_t& outTime)
{
	struct tm ltime;
	memset(&ltime, 0, sizeof(ltime));
	ltime.tm_mday = day;
	ltime.tm_mon = month - 1;
	ltime.tm_year = year - 1900;
	ltime.tm_hour = hour;
	ltime.tm_min = min;
	ltime.tm_sec = sec;

	outTime = mktime(&ltime);
	if (outTime < 0)
	{
		return false;
	}

	// further checking for valid values, including day within month
	// e.g., 2/29, 4/31, etc
	struct tm* localTime = localtime(&outTime);

	if ((localTime->tm_mon != month - 1) ||
		(localTime->tm_year != year - 1900) ||
		(localTime->tm_mday != day) ||
		(localTime->tm_hour != hour) ||
		(localTime->tm_min != min) ||
		(localTime->tm_sec != sec))
	{
		return false;
	}
	return true;
}

bool StringHelper::isTimeConvert(  const std::string &pString,
							time_t &outdate)
{
	if (pString.length() != DATE_TIME_LENGTH) // must be of format YYYYMMDDHHMMSS
	{
		return false;
	}

	string sYear = pString.substr(YEAR_POS, YEAR_LENGTH);
	string sMonth = pString.substr(MONTH_POS, MONTH_LENGTH);
	string sDay = pString.substr(DAY_POS, DAY_LENGTH);
	string sHour = pString.substr(HOUR_POS, HOUR_LENGTH);
	string sMinute = pString.substr(MINUTE_POS, MINUTE_LENGTH);
	string sSecond = pString.substr(SECOND_POS, SECOND_LENGTH);

	long year, month, day, hour, min, sec;
	if ((isNumeric(sYear, year) == false) ||
		(isNumeric(sMonth, month) == false) ||
		(isNumeric(sDay, day) == false) ||
		(isNumeric(sHour, hour) == false) ||
		(isNumeric(sMinute, min) == false) ||
		(isNumeric(sSecond, sec) == false))
	{
		return false;
	}

	if (isValidTime(year, month, day, hour, min, sec, outdate) == false)
	{
		return false;
	}

	return true;
}


std::string StringHelper::convertInteger(int val)
{
	ostringstream ossData;
	ossData << val;
	return ossData.str();
}

std::string StringHelper::convertBoolean(bool val)
{
	ostringstream ossData;
	ossData << val;
	return ossData.str();
}

char* StringHelper::trimString(char *str)
{
	while (*str == ' ') // skip leading spaces
	{
		++str;
	}

	// trim trailing spaces

	int length = strlen(str);
	int index = length - 1;
	for (; index >= 0; --index)
	{
		if (str[index] != ' ')
		{
			break;
		}
	}
	str[index + 1] = 0; // terminate string
	return str;
}

std::string StringHelper::trimString(const std::string& str)
{
	unsigned int first = str.find_first_not_of(" ");
	if (first == string::npos)
	{
		return str.substr(0, 0);
	}
	int last  = str.find_last_not_of(" ");
	return str.substr(first, last - first + 1);
}


bool StringHelper::isConvertToTimeb(const std::string &pString,
							timeb &outdate)
{
        time_t curtime;
        if (true == StringHelper::isTimeConvert(pString, curtime))
        {
        	outdate.time = curtime;
            outdate.millitm = 0;
            return true;
        }
        else
        {
            outdate = TA_Base_Core::AlarmConstants::defaultTime;
            return false;
        }	
}
