/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/active_plans_display/src/StringUtilities.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 13:47:47 $
  * Last modified by:  $Author: builder $
  * 
  * Collection of Static String related functions related to the display of Active
  * Plan Instances.
  */

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "StringUtilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

std::map<std::string, int> TA_Base_Bus::StringUtilities::s_executionStateMap;

static const char PLAN_PATH_SEPARATOR = '/';
static const char *const RUNNING_PLAN_EXEC_STATE = "Running";
static const char *const OPERATOR_PAUSED_EXEC_STATE = "Running";  // Still show plan as running when it's paused for interaction
static const char *const STOPPED_PLAN_EXEC_STATE = "Stopped";
static const char *const PAUSED_PLAN_EXEC_STATE = "Paused";
static const char *const DELAYED_PLAN_EXEC_STATE = "Running";  // Still show plan as running when it's in a delay
static const char *const COMPLETED_PLAN_EXEC_STATE = "Completed";
static const char *const ABORTED_PLAN_EXEC_STATE = "Aborted";
static const char *const FAILED_PLAN_EXEC_STATE = "Failed";
static const char *const PENDING_PLAN_EXEC_STATE = "Pending";
static const char *const UNDEFINED_PLAN_EXEC_STATE = "Undefined";

using namespace TA_Base_Bus;


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
        case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
            return CString(OPERATOR_PAUSED_EXEC_STATE);

		case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
			return CString(RUNNING_PLAN_EXEC_STATE);

		case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
            return CString(STOPPED_PLAN_EXEC_STATE);

        case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
            return CString(PAUSED_PLAN_EXEC_STATE);

        case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
            return CString(DELAYED_PLAN_EXEC_STATE);

        case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
            return CString(COMPLETED_PLAN_EXEC_STATE);

        case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
            return CString(ABORTED_PLAN_EXEC_STATE);

        case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
            return CString(FAILED_PLAN_EXEC_STATE);

        case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
            return CString(PENDING_PLAN_EXEC_STATE);
    }

    return CString(UNDEFINED_PLAN_EXEC_STATE);
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

        s_executionStateMap.insert(std::make_pair(RUNNING_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(PAUSED_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(PENDING_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(COMPLETED_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(STOPPED_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(ABORTED_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(FAILED_PLAN_EXEC_STATE, priority++));
        s_executionStateMap.insert(std::make_pair(UNDEFINED_PLAN_EXEC_STATE, priority++));
    }

    std::map<std::string, int>::const_iterator stateIter1 = s_executionStateMap.find((LPCTSTR) str1);
    std::map<std::string, int>::const_iterator stateIter2 = s_executionStateMap.find((LPCTSTR) str2);

    TA_ASSERT(stateIter1 != s_executionStateMap.end(), "Invalid execution state string");
    TA_ASSERT(stateIter2 != s_executionStateMap.end(), "Invalid execution state string");

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
