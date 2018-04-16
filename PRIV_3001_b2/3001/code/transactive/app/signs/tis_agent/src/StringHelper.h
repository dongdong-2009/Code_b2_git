/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/StringHelper.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A place holder for generic string functions
  *
  */

#ifndef _AFX_STRHELPER_H__9558C070_219B_447E_9704_FB2ACB18148A__
#define _AFX_STRHELPER_H__9558C070_219B_447E_9704_FB2ACB18148A__

#include <string>
#include <sys/timeb.h>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

class StringHelper
{
public:
	template<class T> static std::basic_string<T> replaceStr(const std::basic_string<T>& str,
        													 const std::basic_string<T>& search,
        													 const std::basic_string<T>& replace)
	{
		typedef typename std::basic_string<T> string;
		string r;
		typename string::size_type posBegin = 0;
		typename string::size_type posEnd = str.find(search);
		while(posEnd != string::npos)
		{
			r += str.substr(posBegin, posEnd - posBegin);
			r += replace;
			posBegin = posEnd + search.size();
			posEnd = str.find(search, posBegin);
		}
		return r + str.substr(posBegin);
	}
    /**
	  * isNumeric
	  * 
	  * Check whether a string can be converted to number
	  * Method checks only for positive numbers.
	  * @return bool
	  *         true if the string is a valid number
	  * @param : const std::string& inString
	  * @param : long& outval - output numeric value
	  * 
	  */
	static bool isNumeric(const std::string& inString, long& outval);

	/**
	  * isNumeric
	  * 
	  * Check whether a string can be converted to number
	  * 
	  * @return bool 
	  *         true if the string is a valid number
	  * @param : const std::string& inString
	  * @param : double& outval
	  */
	static bool isNumeric(const std::string& inString, double& outval);

	/**
	  * isValidTime
	  * 
	  * check whether a time is valid
	  * 
	  * @return bool 
	  * @param : int year
	  * @param : int month
	  * @param : int day
	  * @param : int hour
	  * @param : int min
	  * @param : int sec
	  */
	static bool isValidTime(int year, int month, int day, int hour, int min, int sec, time_t& outTime);

	/**
	  * isTimeConvert
	  * 
	  * helper function to get (time_t)
	  * 
	  * @return bool - true if time can be retrieve
	  * @param : std::string pString
	  */
	static bool isTimeConvert(  const std::string &pString,
								time_t &outdate);

	/**
	  * convertInteger
	  * 
	  * Convert integer to string
	  * 
	  * @return static std::string 
	  * @param : int val
	  */
	static std::string convertInteger(int val);

	/**
	  * convertBoolean
	  * 
	  * Convert bool to string
	  * 
	  * @return static std::string 
	  * @param : int val
	  */
	static std::string convertBoolean(bool val);

	/**
	  * trimString
	  * 
	  * Helper function to trim leading and trailing spaces of a string.
	  * This function modifies the input string.
	  *
	  * @return char* 
	  * @param : char *str
	  */
	static char* trimString(char *str);


	/**
	  * trimString
	  * 
	  * Helper function to trim leading and trailing spaces of a string
	  * 
	  * @return std::string
	  * @param : std::string
	  */
	static std::string trimString(const std::string& str);


	/**
	  * isConvertToTimeb
	  * 
	  * helper function to get (timeb)
	  * 
	  * @return bool - true if time can be retrieve
	  * @param : std::string pString
	  */
	static bool isConvertToTimeb(const std::string &pString, timeb &outdate);

    // convert time from '20111222194743' to '22 Dec 2011 19:47:43'	
    static std::string convertTimeFormat(std::string timeStr);


    static std::string generateStnPidPairsString(const std::string& locationName, const TA_Base_Core::PIDList& pidList);

	// convert ttis library string to unsigned long number
	// eg."005003008000"(5.3.8.0 ), convert to unsigned long is:
	// (0 << 24) + (8 << 16) + (3 << 8) + 5
	static unsigned long libraryVersionStrToNumber(const std::string versionStr);
	// reverse function to upon
    static std::string libraryVersionNumberToStr(unsigned long versionNumber);

	static std::string getNewTimestampString();

	static bool isStr1EndWithStr2(const std::string& s1, const std::string& s2);

public:
    static const int DATE_TIME_LENGTH;
	static const int YEAR_LENGTH;
	static const int MONTH_LENGTH;
	static const int DAY_LENGTH;
	static const int HOUR_LENGTH;
	static const int MINUTE_LENGTH;
	static const int SECOND_LENGTH;

	static const int YEAR_POS;
	static const int MONTH_POS;
	static const int DAY_POS;
	static const int HOUR_POS;
	static const int MINUTE_POS;
	static const int SECOND_POS;
};


#endif
