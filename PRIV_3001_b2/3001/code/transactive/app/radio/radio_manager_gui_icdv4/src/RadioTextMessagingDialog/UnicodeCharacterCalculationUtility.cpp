/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/UnicodeCharacterCalculationUtility.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/UnicodeCharacterCalculationUtility.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UnicodeCharacterCalculationUtility::UnicodeCharacterCalculationUtility()
{

}

UnicodeCharacterCalculationUtility::~UnicodeCharacterCalculationUtility()
{

}


unsigned long UnicodeCharacterCalculationUtility::StringSizeInBytes(CString& instring)
{
	return instring.GetLength();
}

std::string UnicodeCharacterCalculationUtility::StringCharacterSetUsed(CString& instring)
{
	// TEMP
	instring = "ASCII";
	
	return instring;
}

unsigned long UnicodeCharacterCalculationUtility::StringLengthOfMsgInDestinationCharSet(CString& instring)
{
	return 0;	
}