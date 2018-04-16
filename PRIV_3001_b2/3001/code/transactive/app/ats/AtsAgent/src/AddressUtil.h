/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AddressUtil.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * Utility functions for ATS DataPoint Addresses
  */
#ifndef ADDRESSUTIL_H
#define ADDRESSUTIL_H

#include <string>
#include <vector>

namespace TA_IRS_App
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
