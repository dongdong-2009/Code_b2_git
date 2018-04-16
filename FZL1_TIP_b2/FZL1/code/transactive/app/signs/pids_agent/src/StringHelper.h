/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/StringHelper.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  * A place holder for generic string functions
  *
  */

#ifndef _AFX_STRHELPER_H__9558C070_219B_447E_9704_FB2ACB18148A__
#define _AFX_STRHELPER_H__9558C070_219B_447E_9704_FB2ACB18148A__

#include <string>
#include <sys/timeb.h>

class StringHelper
{
public:
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
