/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/AddressUtil.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * Utility functions for ATS DataPoint Addresses
  */
#ifndef ADDRESSUTIL_H
#define ADDRESSUTIL_H

#include <string>
#include <vector>

namespace TA_IRS_Bus
{
    class AddressUtil
    {
    public:

		static std::string					getTableName ( const std::string & address );
		static unsigned long				getTableRecordKey ( const std::string & address );
		static std::string					getTableRecordAttribute ( const std::string & address );

		static std::vector <std::string>	splitColonSeparatedString (std::string address);
		static std::string					getColonSeparatedStringItem (std::string address, unsigned int item);
		static std::string					getColonSeparatedStringRemainder (std::string address, unsigned int item);

		static bool							caseInsensitiveCompare(const std::string& s1, const std::string& s2);

	private:
		static std::string					toupper(const std::string& s);
	};
}

#endif 
