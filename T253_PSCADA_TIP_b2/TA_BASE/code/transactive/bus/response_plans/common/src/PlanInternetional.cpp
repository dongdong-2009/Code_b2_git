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
    ActivePlanListColPlan = "预案";			    // Column 1 - Plan
    ActivePlanListColCategory = "类型";	    // Column 2 - Category
    ActivePlanListColInstance = "实例";	    // Column 3 - Instance
    ActivePlanListColLocation = "位置";       // Column 4 - Location
    ActivePlanListColOwner = "用户";			    // Column 5 - Owner
    ActivePlanListColActivation = "激活";	// Column 6 - Activation time
    ActivePlanListColState = "情形";		    	// Column 7 - State
    ActivePlanListColStep = "步骤";		    	// Column 8 - Step
    ActivePlanListColRemarks = "备注";		    // Column 9 - Remarks

    PlanTreeRootItem = "预案";

    ActivePlanStateRunning = "运行";
    ActivePlanStateStpped = "停止";
    ActivePlanStatePaused = "暂停";
    ActivePlanStateCompleted = "完成";
    ActivePlanStatePending = "待定";
    ActivePlanStateUndefined = "未定义";
}

}
