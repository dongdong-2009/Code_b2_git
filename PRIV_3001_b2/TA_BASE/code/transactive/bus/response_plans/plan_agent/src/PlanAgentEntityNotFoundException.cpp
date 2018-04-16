//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
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
