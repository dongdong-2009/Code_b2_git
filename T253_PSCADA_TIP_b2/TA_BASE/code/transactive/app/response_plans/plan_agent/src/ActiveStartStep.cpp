/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStartStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/03/20 14:16:56 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActiveStartStep.h"

namespace TA_Base_App
{

ActiveStartStep::ActiveStartStep(const BasicParameter& detail, IActivePlan& plan) : ActiveStep( m_params, plan ),
m_params(detail)
{
    // Nothing should be done in Start Step
    //m_bCompleted = true;
}

bool ActiveStartStep::executeImpl()
{
    return true;
}

} // TA_Base_App
