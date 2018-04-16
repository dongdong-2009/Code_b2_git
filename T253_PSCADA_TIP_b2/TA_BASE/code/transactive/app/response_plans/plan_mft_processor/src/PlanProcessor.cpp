/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanProcessor.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/12 14:18:48 $
  * Last modified by:  $Author: hoa.le $
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
        ActivePlanAgency::getInstance().init();
    }

    void PlanProcessor::uninit()
    {
        ActivePlanAgency::destroyInstance();
    }

    std::string PlanProcessor::getDLLName() const
    {
        return "TA_Plan_Mft_Step_Message.dll";
    }

}