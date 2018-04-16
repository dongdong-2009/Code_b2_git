/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioPredefinedTextMessage.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioPredefinedTextMessage class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioPredefinedTextMessage.h"


namespace TA_IRS_Core
{
    RadioPredefinedTextMessage::RadioPredefinedTextMessage(const unsigned long key)
    :   m_radioTextHelper(key)
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessage ctor()");
        FUNCTION_EXIT;
    }
    
    RadioPredefinedTextMessage::RadioPredefinedTextMessage(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioTextHelper(row, data)
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessage ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    RadioPredefinedTextMessage::~RadioPredefinedTextMessage()
    {
        FUNCTION_ENTRY("~RadioPredefinedTextMessage");
        FUNCTION_EXIT;
    }

    unsigned long RadioPredefinedTextMessage::getOperator() 
    {
        FUNCTION_ENTRY("getOperator");

        FUNCTION_EXIT;
        return m_radioTextHelper.getOperator();
    }

    std::string RadioPredefinedTextMessage::getShortName()
    {
        FUNCTION_ENTRY("getShortName");

        FUNCTION_EXIT;
        return m_radioTextHelper.getShortName();
    }

    std::string RadioPredefinedTextMessage::getMessageText()
    {
        FUNCTION_ENTRY("getMessageText");

        FUNCTION_EXIT;
        return m_radioTextHelper.getMessageText();
    }


    unsigned long RadioPredefinedTextMessage::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_radioTextHelper.getKey();
    }


    void RadioPredefinedTextMessage::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_radioTextHelper.invalidate();
        FUNCTION_EXIT;
    }

	bool RadioPredefinedTextMessage::getType()
	{
        FUNCTION_ENTRY("getType");
        FUNCTION_EXIT;
		//fixed TD14312,14309
		return m_radioTextHelper.getType();
	}
}
