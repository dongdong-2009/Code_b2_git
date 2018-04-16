/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/alarm_rule/src/CCTVRuleData.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"

namespace TA_Base_Core
{
    const std::string CCTVRuleData::RULE_TYPE = "CCTV";

    
    CCTVRuleData::~CCTVRuleData()
    {
    }


    CCTVRuleData::CCTVRuleData(
        const unsigned long key)
        :
        AlarmRuleData( new CCTVRuleHelper(key) )
    {
    }


    CCTVRuleData::CCTVRuleData()
        :
        AlarmRuleData(new CCTVRuleHelper())
    {
    }


    CCTVRuleData::CCTVRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleData(
            new CCTVRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger) )
    {
    }


    CCTVRuleData::CCTVRuleData(
        const CCTVRuleData& theRule)
        :
        AlarmRuleData(
            new CCTVRuleHelper(
                dynamic_cast<CCTVRuleHelper&>( theRule.getHelper() )))
    {
    }


    std::string CCTVRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }

} // closes TA_Base_Core
