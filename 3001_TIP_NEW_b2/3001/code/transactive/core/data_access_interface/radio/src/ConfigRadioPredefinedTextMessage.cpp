/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioPredefinedTextMessage.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  *
  */

#include <sstream>
#include "core/data_access_interface/radio/src/ConfigRadioPredefinedTextMessage.h"


namespace TA_IRS_Core
{
    ConfigRadioPredefinedTextMessage::ConfigRadioPredefinedTextMessage():
        m_radioHelper()
    {
    }

    ConfigRadioPredefinedTextMessage::ConfigRadioPredefinedTextMessage(const unsigned long key):
        m_radioHelper(key)
    {
    }

    ConfigRadioPredefinedTextMessage::ConfigRadioPredefinedTextMessage(const unsigned long row, TA_Base_Core::IData& data):
        m_radioHelper(row, data)
    {
    }

    ConfigRadioPredefinedTextMessage::ConfigRadioPredefinedTextMessage(const unsigned long operatorid, const unsigned long messageKey, const std::string messageName, const std::string messageText):
        m_radioHelper(operatorid, messageKey, messageName, messageText)
    {
    }

    ConfigRadioPredefinedTextMessage::~ConfigRadioPredefinedTextMessage()
    {
    }

    unsigned long ConfigRadioPredefinedTextMessage::getKey()
    {
        return m_radioHelper.getKey();
    }

	bool ConfigRadioPredefinedTextMessage::getType()
	{
		return m_radioHelper.getType();
	}
	
    unsigned long ConfigRadioPredefinedTextMessage::getOperator()
    {
        return m_radioHelper.getOperator();
    }

    std::string ConfigRadioPredefinedTextMessage::getShortName()
    {
        return m_radioHelper.getShortName();
    }

    std::string ConfigRadioPredefinedTextMessage::getMessageText()
    {
        return m_radioHelper.getMessageText();
    }

    void ConfigRadioPredefinedTextMessage::setOperator(unsigned long operatorId)
    {
        m_radioHelper.setOperator(operatorId);
    }

    void ConfigRadioPredefinedTextMessage::setShortName(const std::string& shortName)
    {
        m_radioHelper.setShortName(shortName);
    }

    void ConfigRadioPredefinedTextMessage::setType(bool bType)
    {
        m_radioHelper.setType(bType);
    }

    void ConfigRadioPredefinedTextMessage::setMessageText(const std::string& messageText)
    {
        m_radioHelper.setMessageText(messageText);
    }

    void ConfigRadioPredefinedTextMessage::invalidate()
    {
        m_radioHelper.invalidate();
    }

    void ConfigRadioPredefinedTextMessage::save()
    {
        m_radioHelper.writePredefinedMessageData();
    }

}

