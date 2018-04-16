/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/Utilities.h $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/12/02 18:24:49 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is a utility class. It provides a set of general purpose methods,
  * mainly for string conversion.
  *
  **/

#if !defined(AFX_UTILITIES_H__00155B08_6693_423D_A826_70DC4846A132__INCLUDED_)
#define AFX_UTILITIES_H__00155B08_6693_423D_A826_70DC4846A132__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // Utilities

    class Utilities
    {
    // Operations
    public:
        static std::string getCurrentSessionId();

        static std::string convertToString(const TA_Base_Core::Seconds delay);
        static std::string convertToString(const TA_Base_Core::EApprovalState appState);
        static void convertFromString(std::string str, double& value, unsigned long& precision);
        static void convertFromString(std::string str, bool& value);
        static void convertFromString(std::string strSrc, int& nValue);

        static int compareUpdateIds(const TA_Base_Core::UpdateId updateId1, const TA_Base_Core::UpdateId updateId2);

        static bool isPlanManagerConfigurationMode();

    private:
        static std::string getString(UINT stringResourceID, std::string defaultString);
    };
}

#endif // !defined(AFX_UTILITIES_H__00155B08_6693_423D_A826_70DC4846A132__INCLUDED_)
