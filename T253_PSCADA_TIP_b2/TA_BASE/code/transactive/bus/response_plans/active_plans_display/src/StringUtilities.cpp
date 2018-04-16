/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/active_plans_display/src/StringUtilities.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/12 14:18:48 $
  * Last modified by:  $Author: hoa.le $
  * 
  * Collection of Static String related functions related to the display of Active
  * Plan Instances.
  */

#include "StdAfx.h"
#include "ace/OS.h"

#include "core/utilities/src/DateFormat.h"
#include "bus/response_plans/common/src/PlanInternetional.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"

namespace TA_Base_Bus
{

std::map<std::string, int> TA_Base_Bus::StringUtilities::s_executionStateMap;

static const char PLAN_PATH_SEPARATOR = '/';



/////////////////////////////////////////////////////////////////////////////
// StringUtilities


CString StringUtilities::extractPlanName(const CString &planPath)
{
    int separatorPos = planPath.ReverseFind(PLAN_PATH_SEPARATOR);
    int planNameStartPos = separatorPos < 0? 0 : separatorPos + 1;

    return planPath.Mid(planNameStartPos);
}


CString StringUtilities::extractCategoryPath(const CString &planPath)
{
    int separatorPos = planPath.ReverseFind(PLAN_PATH_SEPARATOR);
    int categoryPathLen = separatorPos < 0? 0 : (separatorPos == 0? 1 : separatorPos);

    return planPath.Left(categoryPathLen);    
}


CString StringUtilities::convertToString(const int value)
{
    CString valueString;

    valueString.Format("%d", value);

    return valueString;
}


CString StringUtilities::convertToString(const unsigned long value)
{
    CString valueString;

    valueString.Format("%lu", value);

    return valueString;
}


CString StringUtilities::convertToString(const double value, const unsigned long precision)
{
    CString valueString;
    
    valueString.Format("%.*lf", precision, value);

    return valueString;
}

CString StringUtilities::convertToString(const TA_Base_Core::EActivePlanState execState)
{
    switch (execState)
    {
		case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
            return PlanInternetional::ActivePlanStateRunning.c_str();

		case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
            return PlanInternetional::ActivePlanStateStpped.c_str();

        case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
            return PlanInternetional::ActivePlanStatePaused.c_str();

        case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
            return PlanInternetional::ActivePlanStateCompleted.c_str();

        case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
            return PlanInternetional::ActivePlanStatePending.c_str();
    }

    return PlanInternetional::ActivePlanStateUndefined.c_str();
}


int StringUtilities::compareNumbers(const CString &str1, const CString &str2)
{
    char *end = NULL;
    double num1 = ::strtod(str1, &end);
    double num2 = ::strtod(str2, &end);

    if (num1 < num2)
    {
        return -1;
    }

    if (num1 > num2)
    {
        return 1;
    }
 
    return 0;
}


int StringUtilities::compareTimestamps(const CString &str1, const CString &str2)
{
    COleDateTime time1, time2;
    time1.ParseDateTime(str1);
    time2.ParseDateTime(str2);

    if (time1 < time2)
    {
        return -1;
    }

    if (time1 > time2)
    {
        return 1;
    }
 
    return 0;
}


int StringUtilities::compareExecutionStates(const CString &str1, const CString &str2)
{
    if (s_executionStateMap.empty())
    {
        int priority = 0; // 0 = highest

        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStateRunning, priority++));
        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStatePaused, priority++));
        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStatePending, priority++));
        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStateCompleted, priority++));
        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStateStpped, priority++));
        s_executionStateMap.insert(std::make_pair(PlanInternetional::ActivePlanStateUndefined, priority++));
    }

    std::map<std::string, int>::const_iterator stateIter1 = s_executionStateMap.find((LPCTSTR) str1);
    std::map<std::string, int>::const_iterator stateIter2 = s_executionStateMap.find((LPCTSTR) str2);

    if ( stateIter1 == s_executionStateMap.end() )
    {
        return 1;
    }

    if ( stateIter2 == s_executionStateMap.end() )
    {
        return -1;
    }

    int state1Priority = stateIter1->second;
    int state2Priority = stateIter2->second;

    if (state1Priority < state2Priority)
    {
        return -1;
    }

    if (state1Priority > state2Priority)
    {
        return 1;
    }
 
    return 0;    
}

std::string TA_Base_Bus::StringUtilities::timeToString( const time_t tmSrc )
{
    static const size_t MAX_TIME_LENGTH = 40;
    TA_Base_Core::DateFormat dateFormat; 
    std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);

    struct tm newtime;
    ACE_OS::localtime_r( &tmSrc, &newtime ); // Convert to local time.
    char localTime[MAX_TIME_LENGTH];
    ACE_OS::strftime(localTime, MAX_TIME_LENGTH, dateFormatStr.c_str(), &newtime);

    return static_cast<const char*>(localTime);
}

}