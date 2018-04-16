/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/alarm_rule/src/CCTVRuleHelper.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 14:06:39 $
  * Last modified by:  $Author: huirong.luo $
  *
  * Provides an interface to Alarm Rules of type CCTV rule in the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/alarm_rule/src/CCTVRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    CCTVRuleHelper::CCTVRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleHelper(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
            CCTVRuleData::RULE_TYPE,
            ruleTrigger)
    {
    }


    CCTVRuleHelper::CCTVRuleHelper()
        :
        AlarmRuleHelper(
            CCTVRuleData::RULE_TYPE,
            IAlarmRuleData::RULE_TRIGGER_SUBMIT)
    {
    }


    CCTVRuleHelper::CCTVRuleHelper(unsigned long pkey)
        :
        AlarmRuleHelper(pkey)
    {
    }


    CCTVRuleHelper::CCTVRuleHelper(
        CCTVRuleHelper& original)
        :
        AlarmRuleHelper(
            original.getEntityTypeKey(),
            original.getEntityKey(),
            original.getAlarmType(),
            original.getRuleType(),
            original.getRuleTrigger())
    {
    }


    CCTVRuleHelper::~CCTVRuleHelper()
    {
    }


}// TA_Base_Core
