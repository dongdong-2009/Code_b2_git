/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/CombinedEventData.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is an implementation of the ICombinedEvent interface. It provides all
  * the data required by the Event Viewer from both the EVENT and LO_DATAPT_STATE_CHANGE
  * tables.
  *
  * All data is populated when the object is constructed and is read-only.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include <memory>
#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;

namespace TA_Base_Core
{
    const ECombinedEventType CombinedEventData::getEventType()
    {
        return m_type;
    }


    const unsigned long CombinedEventData::getKey()
    {
        return m_key;
    }


    const timeb CombinedEventData::getCreateTime()
    {
        return m_createTime;
    }


    const std::string& CombinedEventData::getAlarmSeverityName()
    {
        return m_alarmSeverityName;
    }


    const std::string& CombinedEventData::getAssetName()
    {
        return m_assetName;
    }


    const std::string& CombinedEventData::getDescription()
    {
        return m_description;
    }


    const std::string& CombinedEventData::getValue()
    {
        return m_value;
    }


    const std::string& CombinedEventData::getAlarmMmsState()
    {
        return m_alarmMmsState;
    }


    const std::string& CombinedEventData::getAlarmDssState()
    {
        return m_alarmDssState;
    }


    const std::string& CombinedEventData::getAlarmAvlState()
    {
        return m_alarmAvlState;
    }


    const std::string& CombinedEventData::getOperatorName()
    {
        return m_operatorName;        
    }


    const std::string& CombinedEventData::getAlarmComment()
    {
        return m_alarmComment;
    }

    const std::string& CombinedEventData::getAlarmId()
    {
        return m_alarmId;
    }

    const std::string& CombinedEventData::getAlarmAck()
    {
        return m_alarmAck;
    }

    const std::string& CombinedEventData::getAlarmStatus()
    {
        return m_alarmStatus;
    }

};