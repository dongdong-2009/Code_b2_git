/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/StringHelper.cpp $
  * @author:  Albertus Zakaria
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  *
  * A place holder for generic string functions
  *
  */


#include <iomanip>
#include <sstream>
#include <iostream>
#include <time.h>
#include "app/signs/tis_agent/src/StringHelper.h"
#include "core/alarm/src/AlarmConstants.h"
#include <ace/OS.h>

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
	std::string::size_type first = str.find_first_not_of(" ");
	if (first == string::npos)
	{
		return str.substr(0, 0);
	}
	std::string::size_type last  = str.find_last_not_of(" ");
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

std::string StringHelper::convertTimeFormat(std::string timeStr)
{
	const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::stringstream ss;
	int day, month;
	if(timeStr.size() != 14)
	{
		return ss.str();
	}	

	ss << timeStr.substr(4, 2); //month
	ss >> month;
	ss.clear();
	ss.str("");
	if(month < 1 || month > 12)
	{
		return ss.str();
	}

	ss << timeStr.substr(6, 2); //day
	ss >> day;
	ss.clear();
	ss.str("");

	ss << day << " ";
	ss << months[month-1] << " ";
	ss << timeStr.substr(0, 4) << " ";

	std::string dayTime = timeStr.substr(8, 6);
	dayTime.insert(2, 1, ':');
	dayTime.insert(5, 1, ':');
	ss << dayTime;

	return ss.str();
}

std::string StringHelper::generateStnPidPairsString(const std::string& locationName, const TA_Base_Core::PIDList& pidList)
{
	int pidLenth = pidList.length();
	std::stringstream ss;
	for(int i = 0; i < pidLenth; ++i)
	{
		ss << locationName << " " << pidList[i];
		if(i < pidLenth-1)
		{
			ss << ", ";
		}
	}
	return ss.str();
}

unsigned long StringHelper::libraryVersionStrToNumber(const std::string versionStr)
{
	if( 12 != versionStr.size() )
	{
		return 0;
	}
	unsigned long sum = 0;
	unsigned long tmp = 0;
	std::stringstream ss;
	for(int i = 0; i < 12; i+=3)
	{
		ss.clear();
		ss.str("");
		ss << versionStr.substr(i, 3);
		ss >> tmp;
		sum += (tmp << (24 - (i/3)*8));
	}
	return sum;
}

std::string StringHelper::libraryVersionNumberToStr(unsigned long versionNumber)
{
	unsigned char* valueStr = (unsigned char*)&versionNumber;
	std::stringstream ss;	
	for(int i = 3; i >= 0; --i)
	{
		ss << std::setfill('0') << std::setw(3) << (unsigned short)valueStr[i];		
	}
	return ss.str();	
}

std::string StringHelper::getNewTimestampString()
{
    std::stringstream timeStamp;

    time_t currentTime;
    tm localTime;

    currentTime = time( NULL );
    ACE_OS::localtime_r( &currentTime, &localTime );

    // the timestamp is in the format
    // YYYYMMDDHHmmSS

    // YYYY
    timeStamp << ( localTime.tm_year + 1900 );

    // MM
    // tm_mon is months since january, so 0 = january
    timeStamp << std::setw( 2 ) << std::setfill( '0' ) << ( localTime.tm_mon + 1 );

    // DD
    // tm_mday is the day of the month (1-31)
    timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_mday;

    // HH
    // tm_hour is the hours past the day (0-23)
    timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_hour;

    // MM
    // tm_min is the minutes past the hour (0-59)
    timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_min;

    // SS
    // tm_sec is the seconds past the minute (0-60) (61 for leap seconds)
    timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_sec;

    return timeStamp.str();
}

bool StringHelper::isStr1EndWithStr2( const std::string& s1, const std::string& s2 )
{
	std::string::size_type pos = s1.rfind(s2);
	if(pos != std::string::npos)
	{
		return s1.length() - s2.length() == pos;
	}
	return false;
}