/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_tree/src/PlanAgentException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Thrown by AgentPlanAccess when an error occurs while attempting to communicate with the Plan Agent.
  */

#include "bus\response_plans\plan_tree\src\PlanAgentException.h"


using namespace TA_Base_Bus;


PlanAgentException::PlanAgentException() throw()
{
}


PlanAgentException::PlanAgentException(const std::string& msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanAgentException::~PlanAgentException() throw()
{
}
