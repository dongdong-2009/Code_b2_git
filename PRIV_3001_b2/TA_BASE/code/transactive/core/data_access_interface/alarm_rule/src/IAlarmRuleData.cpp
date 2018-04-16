/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/alarm_rule/src/IAlarmRuleData.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * IAlarmRuleData is an interface to the AlarmRule table. It provides
  * access for existing alarm rules, and the ability to create new alarm rules.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif



#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"

namespace TA_Base_Core
{
    // The allowable values for the rule trigger
    const std::string IAlarmRuleData::RULE_TRIGGER_ACKNOWLEDGE = "acknowledge";
    const std::string IAlarmRuleData::RULE_TRIGGER_SUBMIT      = "submit";
    const std::string IAlarmRuleData::RULE_TRIGGER_CLOSE       = "close";

} // Closes TA_Base_Core


