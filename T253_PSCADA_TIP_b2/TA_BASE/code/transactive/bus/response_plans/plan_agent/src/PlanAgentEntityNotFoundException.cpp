//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2017/12/12 14:18:48 $
// Last modified by:  $Author: hoa.le $
//
// Thrown by PlanAgentLibrary when no entity of type PlanAgent can be found at the specified location.

#include "PlanAgentEntityNotFoundException.h"

using namespace TA_Base_Bus;


PlanAgentEntityNotFoundException::PlanAgentEntityNotFoundException() throw() : 
    TA_Base_Core::TransactiveException()
{
}


PlanAgentEntityNotFoundException::PlanAgentEntityNotFoundException(std::string msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanAgentEntityNotFoundException::~PlanAgentEntityNotFoundException() throw()
{
}
