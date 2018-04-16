#ifndef ACTIVEENDSTEP_HEADER_INCLUDED
#define ACTIVEENDSTEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveEndStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2015/01/08 17:33:12 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActiveEndStep.h"

namespace TA_Base_App
{

ActiveEndStep::ActiveEndStep(const BasicParameter& detail, IActivePlan& plan) : ActiveStep( m_params, plan ),
m_params( detail )
{
}

bool ActiveEndStep::executeImpl()
{
    return true;
}

} // TA_Base_App

#endif