#ifndef ACTIVE_PARALLEL_GATEWAY_H_INCLUDED
#define ACTIVE_PARALLEL_GATEWAY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveParallelGateway.h $
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
class ActiveParallelGateway : public ActiveStep
{
// Public methods
public:
    ActiveParallelGateway( const BasicParameter& refDetail, IActivePlan& iPlan );
    virtual std::list<ActiveStep*> getNextStep();

protected:
    virtual void initImpl();
    //virtual bool checkImpl();
	virtual bool executeImpl();
    virtual int waitImpl();

private:
	ActiveParallelGateway& operator=( const ActiveParallelGateway& );
	ActiveParallelGateway( const ActiveParallelGateway& );

private:
    bool m_bNextReturned;
    size_t m_szJointTimes;
	EmptyStepParameter m_params;
}; // ActiveParallelGateway

}; // TA_Base_App

#endif // ACTIVE_PARALLEL_GATEWAY_H_INCLUDED
