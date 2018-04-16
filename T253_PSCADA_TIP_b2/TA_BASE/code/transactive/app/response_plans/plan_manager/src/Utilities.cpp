/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/Utilities.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/12/02 18:24:49 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is a utility class. It provides a set of general purpose methods,
  * mainly for string conversion.
  *
  **/

#include "stdafx.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"
#include "PlanManager.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// Utilities

std::string Utilities::getCurrentSessionId()
{
    FUNCTION_ENTRY( "getCurrentSessionId" );
    FUNCTION_EXIT;
    return TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
}

std::string Utilities::convertToString(const TA_Base_Core::Seconds delay)
{
    FUNCTION_ENTRY( "convertToString" );

    time_t minutes = delay / 60;
    time_t seconds = delay % 60;

    std::ostringstream minAndSec;
    minAndSec << std::setw(2) << std::setfill('0') << minutes << ':' << std::setw(2) << std::setfill('0') << seconds;

    FUNCTION_EXIT;
    return minAndSec.str();
}

std::string Utilities::convertToString(const TA_Base_Core::EApprovalState appState)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (appState)
    {
        case TA_Base_Core::APPROVED_APPROVAL_STATE:
            FUNCTION_EXIT;
            return getString(IDS_PLAN_APPROVED_STATUS, "Approved");

        case TA_Base_Core::UNAPPROVED_APPROVAL_STATE:
            FUNCTION_EXIT;
            return getString(IDS_PLAN_UNAPPROVED_STATUS, "Unapproved");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_PLAN_UNDEFINED_APPROVAL_STATUS, "Undefined");
}

void Utilities::convertFromString(std::string str, double& value, unsigned long& precision)
{
    FUNCTION_ENTRY( "convertFromString" );

    value = atof(str.c_str());

    std::string::size_type decPointPos = str.find_last_of('.');
    if (decPointPos == std::string::npos)
    {
        precision = 0;
    }
    else
    {
        std::string::size_type lastDigitPos = str.find_last_not_of(" \t");
        precision = lastDigitPos - decPointPos;
    }

    FUNCTION_EXIT;
}

void Utilities::convertFromString(std::string str, bool &value)
{
    FUNCTION_ENTRY( "convertFromString" );

    value = (str != "= 0" && str[0] != 'f' && str[0] != 'F');

    FUNCTION_EXIT;
}


void TA_Base_App::Utilities::convertFromString( std::string strSrc, int& nValue )
{
    FUNCTION_ENTRY( "convertFromString" );

    std::stringstream stmConvertor;
    stmConvertor << strSrc;
    stmConvertor >> nValue;

    FUNCTION_EXIT;
}

bool Utilities::isPlanManagerConfigurationMode()
{
    FUNCTION_ENTRY( "isPlanManagerConfigurationMode" );

    static const char* const PLAN_MANAGER_SYSTEM_MODE_STRING = "Admin";
    static const bool bPlanManagerMode = TA_Base_Core::RunParams::getInstance().isSet(PLAN_MANAGER_SYSTEM_MODE_STRING);

    FUNCTION_EXIT;
    return bPlanManagerMode;
}

// Return:  0 if updateId1 is same as updateId2
//         -1 if updateId1 is less recent than updateId2
//         +1 if updateId1 is more recent than updateId2
int Utilities::compareUpdateIds(const TA_Base_Core::UpdateId updateId1, const TA_Base_Core::UpdateId updateId2)
{
    FUNCTION_ENTRY( "compareUpdateIds" );

    static const double WRAPAROUND_GAP = (TA_Base_Core::MAX_UPDATE_ID - TA_Base_Core::MIN_UPDATE_ID) / 2;

    if (updateId2 == updateId1)
    {
        FUNCTION_EXIT;
        return 0;
    }

    // If update ID has wrapped around
    if (::fabs((double) updateId2 - (double) updateId1) > WRAPAROUND_GAP)
    {
        FUNCTION_EXIT;
        return (updateId1 < updateId2? 1 : -1);
    }

    FUNCTION_EXIT;
    return (updateId1 > updateId2? 1 : -1);
}


std::string Utilities::getString(UINT stringResourceID, std::string defaultString)
{
    FUNCTION_ENTRY( "getString" );

    CString string;

    if (string.LoadString(stringResourceID) == 0)
    {
        FUNCTION_EXIT;
        return defaultString;
    }

    FUNCTION_EXIT;
    return string.operator LPCSTR();
}
