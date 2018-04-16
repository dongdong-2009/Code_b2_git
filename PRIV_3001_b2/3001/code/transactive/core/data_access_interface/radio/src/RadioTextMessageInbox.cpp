/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInbox.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioTextMessageInbox class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioTextMessageInbox.h"


namespace TA_IRS_Core
{
    RadioTextMessageInbox::RadioTextMessageInbox(const unsigned long key)
    :   m_radioTextHelper(key)
    {
        FUNCTION_ENTRY("RadioTextMessageInbox ctor()");
        FUNCTION_EXIT;
    }
    
    RadioTextMessageInbox::RadioTextMessageInbox(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioTextHelper(row, data)
    {
        FUNCTION_ENTRY("RadioTextMessageInbox ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    RadioTextMessageInbox::~RadioTextMessageInbox()
    {
        FUNCTION_ENTRY("~RadioTextMessageInbox");
        FUNCTION_EXIT;
    }
    
    time_t RadioTextMessageInbox::getTimeStamp()
    {
        FUNCTION_ENTRY("getTimeStamp");

        FUNCTION_EXIT;
        return m_radioTextHelper.getTimeStamp();
    }

    unsigned long RadioTextMessageInbox::getOperator()
    {
        FUNCTION_ENTRY("getOperator");

        FUNCTION_EXIT;
        return m_radioTextHelper.getOperator();
    }

    bool RadioTextMessageInbox::isRead()
    {
        FUNCTION_ENTRY("isRead");

        FUNCTION_EXIT;
        return m_radioTextHelper.isRead();
    }

    std::string RadioTextMessageInbox::getMessageText()
    {
        FUNCTION_ENTRY("getMessageText");

        FUNCTION_EXIT;
        return m_radioTextHelper.getMessageText();
    }

    std::string RadioTextMessageInbox::getSourceAlias()
    {
        FUNCTION_ENTRY("getMessageText");

        FUNCTION_EXIT;
        return m_radioTextHelper.getSourceAlias();
    }

    std::string RadioTextMessageInbox::getRecipientConsole()
    {
        FUNCTION_ENTRY("getRecipientConsole");

        FUNCTION_EXIT;
        return m_radioTextHelper.getRecipientConsole();
    }

    unsigned long RadioTextMessageInbox::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_radioTextHelper.getKey();
    }


    void RadioTextMessageInbox::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_radioTextHelper.invalidate();
        FUNCTION_EXIT;
    }
}
