/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanProcessor.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "app/response_plans/plan_mft_processor/src/PlanProcessor.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"

namespace TA_Base_App
{

    PlanProcessor::PlanProcessor()
    {
    }

    PlanProcessor::~PlanProcessor()
    {
    }

    void PlanProcessor::init( TA_Base_App::IDllMgrCallBack* callback )
    {
        ActivePlanAgency::instance()->init();
    }

    void PlanProcessor::uninit()
    {
        ActivePlanAgency::instance()->uninit();

        ActivePlanAgency::close();
    }

    std::string PlanProcessor::getDLLName() const
    {
        return "TA_Plan_Mft_Step_Message.dll";
    }

}