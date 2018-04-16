/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/UnicodeCharacterCalculationUtility.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This utility provides Unicode character calculations
 */

#if !defined(AFX_UNICODECHARACTERCALCULATIONUTILITY_H__923BAE55_EA88_42C5_BE1B_41D126139E35__INCLUDED_)
#define AFX_UNICODECHARACTERCALCULATIONUTILITY_H__923BAE55_EA88_42C5_BE1B_41D126139E35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

class UnicodeCharacterCalculationUtility  
{
	public:
		UnicodeCharacterCalculationUtility();
		virtual ~UnicodeCharacterCalculationUtility();

	public:
		/**
		 * This will Determine the size of the provided unicode string in bytes. This
		 * method will effectively just have to be a normal sizeof of the string until the
		 * Unicode implementation is complete. This method needs to be upgraded to
		 * included proper Unicode counts by the Unicode implementors.
		 */
		static unsigned long StringSizeInBytes(CString& instring);
		/**
		 * This will Determine the character set used of the provided unicode string. This
		 * method will return a textual description of the character set. This method
		 * needs to be implemented by the Unicode implementors.
		 */
		static std::string StringCharacterSetUsed(CString& instring);
		/**
		 * This will Determine the character set used of the provided unicode string. This
		 * method will return a textual description of the character set. This method
		 * needs to be implemented by the Unicode implementors.
		 */
		static unsigned long StringLengthOfMsgInDestinationCharSet(CString& instring);
};

#endif // !defined(AFX_UNICODECHARACTERCALCULATIONUTILITY_H__923BAE55_EA88_42C5_BE1B_41D126139E35__INCLUDED_)
