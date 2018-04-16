/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/PlanInternetional.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/12 14:04:35 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#include "bus/response_plans/common/src/PlanInternetional.h"

namespace TA_Base_Bus
{

/////////////////////////////////////////////////////////////////////////////
// PlanInternetional

std::string PlanInternetional::ActivePlanListColPlan = "";
std::string PlanInternetional::ActivePlanListColCategory = "";
std::string PlanInternetional::ActivePlanListColInstance = "";
std::string PlanInternetional::ActivePlanListColLocation = "";
std::string PlanInternetional::ActivePlanListColOwner = "";
std::string PlanInternetional::ActivePlanListColActivation = "";
std::string PlanInternetional::ActivePlanListColState = "";
std::string PlanInternetional::ActivePlanListColStep = "";
std::string PlanInternetional::ActivePlanListColRemarks = "";
std::string PlanInternetional::PlanTreeRootItem = "";
std::string PlanInternetional::ActivePlanStateRunning = "";
std::string PlanInternetional::ActivePlanStateStpped = "";
std::string PlanInternetional::ActivePlanStatePaused = "";
std::string PlanInternetional::ActivePlanStateCompleted = "";
std::string PlanInternetional::ActivePlanStatePending = "";
std::string PlanInternetional::ActivePlanStateUndefined = "";

void PlanInternetional::initLocaleEnglish()
{
    ActivePlanListColPlan = "Plan";			    // Column 1 - Plan
    ActivePlanListColCategory = "Category";	    // Column 2 - Category
    ActivePlanListColInstance = "Instance";	    // Column 3 - Instance
    ActivePlanListColLocation = "Location";       // Column 4 - Location
    ActivePlanListColOwner = "Owner";			    // Column 5 - Owner
    ActivePlanListColActivation = "Activation";	// Column 6 - Activation time
    ActivePlanListColState = "State";		    	// Column 7 - State
    ActivePlanListColStep = "Step";		    	// Column 8 - Step
    ActivePlanListColRemarks = "Remarks";		    // Column 9 - Remarks

    PlanTreeRootItem = "Plans";

    ActivePlanStateRunning = "Running";
    ActivePlanStateStpped = "Stopped";
    ActivePlanStatePaused = "Paused";
    ActivePlanStateCompleted = "Completed";
    ActivePlanStatePending = "Pending";
    ActivePlanStateUndefined = "Undefined";
}

void PlanInternetional::initLocaleSimpChinese()
{
    ActivePlanListColPlan = "Ԥ��";			    // Column 1 - Plan
    ActivePlanListColCategory = "����";	    // Column 2 - Category
    ActivePlanListColInstance = "ʵ��";	    // Column 3 - Instance
    ActivePlanListColLocation = "λ��";       // Column 4 - Location
    ActivePlanListColOwner = "�û�";			    // Column 5 - Owner
    ActivePlanListColActivation = "����";	// Column 6 - Activation time
    ActivePlanListColState = "����";		    	// Column 7 - State
    ActivePlanListColStep = "����";		    	// Column 8 - Step
    ActivePlanListColRemarks = "��ע";		    // Column 9 - Remarks

    PlanTreeRootItem = "Ԥ��";

    ActivePlanStateRunning = "����";
    ActivePlanStateStpped = "ֹͣ";
    ActivePlanStatePaused = "��ͣ";
    ActivePlanStateCompleted = "���";
    ActivePlanStatePending = "����";
    ActivePlanStateUndefined = "δ����";
}

}
