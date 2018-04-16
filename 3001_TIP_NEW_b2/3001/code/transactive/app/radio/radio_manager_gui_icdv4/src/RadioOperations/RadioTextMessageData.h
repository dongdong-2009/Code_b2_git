/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageData.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * A placeholder structure for message details
 */

#if !defined(AFX_RADIOTEXTMESSAGEDATA_H__0F108A14_8489_4A98_8FE1_95BC992966D8__INCLUDED_)
#define AFX_RADIOTEXTMESSAGEDATA_H__0F108A14_8489_4A98_8FE1_95BC992966D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

struct RadioTextMessageData
{
    time_t MessageTime;
    std::string MessageSource;
    std::string MessageSourceDescription;
    std::string MessageText;
};


#endif // !defined(AFX_RADIOTEXTMESSAGEDATA_H__0F108A14_8489_4A98_8FE1_95BC992966D8__INCLUDED_)
