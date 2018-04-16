#ifndef ACTIVE_END_STEP_H_INCLUDED
#define ACTIVE_END_STEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveEndStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #9 $
  *
  *  Last modification: $DateTime: 2015/03/18 12:39:25 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"

namespace TA_Base_App
{

//Class declaration
class ActiveEndStep : public ActiveStep
{
    // Public methods
public:
    ActiveEndStep(const BasicParameter& detail, IActivePlan& plan);

protected:
    virtual bool executeImpl();

private:
    ActiveEndStep& operator=(const ActiveEndStep&);
    ActiveEndStep(const ActiveEndStep&);

private:
	EmptyStepParameter m_params;
}; // ActiveEndStep

}; // TA_Base_App

#endif // ACTIVE_END_STEP_H_INCLUDED
