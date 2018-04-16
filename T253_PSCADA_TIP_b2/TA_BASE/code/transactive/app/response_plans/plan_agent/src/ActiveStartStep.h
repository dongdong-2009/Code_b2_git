#ifndef ACTIVE_START_STEP_H_INCLUDED
#define ACTIVE_START_STEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStartStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"

namespace TA_Base_App
{

//Class declaration
class ActiveStartStep : public ActiveStep
{
// Public methods
public:
    ActiveStartStep( const BasicParameter& detail, IActivePlan& plan );
	
protected:
	virtual bool executeImpl();

private:
	ActiveStartStep& operator=( const ActiveStartStep& );
	ActiveStartStep( const ActiveStartStep& );

private:
	EmptyStepParameter m_params;
}; // ActiveStartStep

}; // TA_Base_App

#endif // ACTIVE_START_STEP_H_INCLUDED
