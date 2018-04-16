/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/AddressUtil.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * Utility functions for Ats DataPoint Addresses
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "bus/ats/ats_agent/CommonATS/src/AddressUtil.h"

namespace TA_IRS_Bus
{
	std::string AddressUtil::getTableName ( const std::string & address )
	{
		return getColonSeparatedStringItem(address, 0);
	}

	unsigned long AddressUtil::getTableRecordKey ( const std::string & address )
	{
		unsigned long key = 0;
		std::string keyStr = getColonSeparatedStringItem(address, 1);

		if (keyStr.length() > 0)
		{
			std::istringstream keyStream (keyStr);
			keyStream >> key;
		}
	
		return key;
	}

	std::string AddressUtil::getTableRecordAttribute ( const std::string & address )
	{
		return getColonSeparatedStringItem(address, 2);
	}

	std::vector <std::string> AddressUtil::splitColonSeparatedString (std::string address)
	{
		std::string workingAddress = address;
		std::vector<std::string> result;
		
		result.clear();

		int pos = -1;
		do
		{
			pos = workingAddress.find_first_of( ":", 0 );
			if ( pos >= 0 )
			{
				// store the text before the first ':' in the vector
				result.push_back(workingAddress.substr ( 0, pos ));

				std::string temp = workingAddress.substr ( pos + 1, workingAddress.length() );
				workingAddress = temp;
			}
			else
			{
				result.push_back(workingAddress);
			}
		}
		while (pos >= 0);

		return result;
	}	

	std::string	AddressUtil::getColonSeparatedStringItem (std::string address, unsigned int item)
	{
		std::string workingAddress = address;
		std::string result = "";

		int pos = -1;
		unsigned int itemCount = 0;
		do
		{
			pos = workingAddress.find_first_of( ":", 0 );
			if ( pos >= 0 )
			{
				if (item == itemCount)
				{
					result = workingAddress.substr ( 0, pos );
				}

				std::string temp = workingAddress.substr ( pos + 1, workingAddress.length() );
				workingAddress = temp;
				itemCount++;
			}
			else
			{
				if (item == itemCount)
				{
					result = workingAddress;
				}
				itemCount++;
			}
		}
		while (pos >= 0 && itemCount <= item);

		return result;
	}

	std::string	AddressUtil::getColonSeparatedStringRemainder (std::string address, unsigned int item)
	{
		std::string workingAddress = address;
		std::string result = "";

		int pos = -1;
		unsigned int itemCount = 0;
		do
		{
			pos = workingAddress.find_first_of( ":", 0 );
			if ( pos >= 0 )
			{
				std::string temp = workingAddress.substr ( pos + 1, workingAddress.length() );
				workingAddress = temp;
			}
			
			if (item == itemCount)
			{
				result = workingAddress;
			}
			itemCount++;
		}
		while (pos >= 0 && itemCount <= item);

		return result;
	}

	// Make an uppercase copy of s
	std::string AddressUtil::toupper(const std::string& s) 
	{
		std::string upper(s);
		for(size_t i=0; i<s.length(); i++)
		{
			upper[i] = ::toupper(upper[i]);
		}
		return upper;
	}

	// Case insensitive comparison
	bool AddressUtil::caseInsensitiveCompare(const std::string& s1, const std::string& s2) 
	{
		return (toupper(s1) == toupper(s2));
	}
}
