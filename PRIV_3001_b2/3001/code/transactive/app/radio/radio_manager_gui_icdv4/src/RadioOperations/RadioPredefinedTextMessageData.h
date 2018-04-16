/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioPredefinedTextMessageData.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * A placeholder structure for message details
 */


#if !defined(AFX_RADIOPREDEFINEDTEXTMESSAGEDATA_H__EFB59545_5D9B_44C4_A47E_3A59C3F5DFF1__INCLUDED_)
#define AFX_RADIOPREDEFINEDTEXTMESSAGEDATA_H__EFB59545_5D9B_44C4_A47E_3A59C3F5DFF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

struct RadioPredefinedTextMessageData
{
	/**
	 * Name of the predefined message
	 */
	std::string MessageName;
	/**
	 * Text of the predefined message.
	 */
	std::string MessageText;
};

#endif // !defined(AFX_RADIOPREDEFINEDTEXTMESSAGEDATA_H__EFB59545_5D9B_44C4_A47E_3A59C3F5DFF1__INCLUDED_)
