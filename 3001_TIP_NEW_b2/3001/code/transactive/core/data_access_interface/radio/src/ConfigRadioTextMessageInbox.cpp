/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioTextMessageInbox.cpp $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "core/data_access_interface/radio/src/ConfigRadioTextMessageInbox.h"
#include <sstream>

namespace TA_IRS_Core
{
    ConfigRadioTextMessageInbox::ConfigRadioTextMessageInbox():
        m_radioHelper()
    {
    }

    ConfigRadioTextMessageInbox::ConfigRadioTextMessageInbox(const unsigned long key):
        m_radioHelper(key)
    {
    }

    ConfigRadioTextMessageInbox::ConfigRadioTextMessageInbox(const unsigned long row, TA_Base_Core::IData& data):
        m_radioHelper(row, data)
    {
    }

    ConfigRadioTextMessageInbox::ConfigRadioTextMessageInbox(const time_t timestamp,const unsigned long oper,const char isread,const std::string& msgtext,const std::string& source, const std::string& recipient) :
        m_radioHelper(timestamp, oper, isread, msgtext,source, recipient)
    {
    }

    ConfigRadioTextMessageInbox::~ConfigRadioTextMessageInbox()
    {
    }

    unsigned long ConfigRadioTextMessageInbox::getKey()
    {
        return m_radioHelper.getKey();
    }

    time_t ConfigRadioTextMessageInbox::getTimeStamp()
    {
        return getTimeStamp();
    }
    
    unsigned long ConfigRadioTextMessageInbox::getOperator()
    {
        return m_radioHelper.getOperator();
    }

    bool ConfigRadioTextMessageInbox::isRead()
    {
        return m_radioHelper.isRead();
    }

    std::string ConfigRadioTextMessageInbox::getMessageText()
    {
        return m_radioHelper.getMessageText();
    }

	std::string ConfigRadioTextMessageInbox::getSourceAlias()
	{
		return m_radioHelper.getSourceAlias();
	}

	std::string ConfigRadioTextMessageInbox::getRecipientConsole()
	{
		return m_radioHelper.getRecipientConsole();
	}

    void ConfigRadioTextMessageInbox::setOperator(unsigned long operatorId)
    {
        m_radioHelper.setOperator(operatorId);
    }

    void ConfigRadioTextMessageInbox::setTimeStamp(time_t pTime)
    {
        m_radioHelper.setTimeStamp(pTime);
    }

    void ConfigRadioTextMessageInbox::setRead(bool pRead)
    {
        m_radioHelper.setRead(pRead);
    }

    void ConfigRadioTextMessageInbox::setMessageText(const std::string& messageText)
    {
        m_radioHelper.setMessageText(messageText);
    }

    void ConfigRadioTextMessageInbox::setSourceAlias(const std::string& source)
    {
        m_radioHelper.setSourceAlias(source);
    }

	void ConfigRadioTextMessageInbox::setRecipientConsole(const std::string& recipient)
	{
		m_radioHelper.setRecipientConsole(recipient);
	}

    void ConfigRadioTextMessageInbox::invalidate()
    {
        m_radioHelper.invalidate();
    }

    void ConfigRadioTextMessageInbox::save()
    {
        m_radioHelper.writeTextMessageData();
    }


}
