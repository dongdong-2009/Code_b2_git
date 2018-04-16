/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStepException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Thrown to indicate problems while executing an interactive step.
  */

#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


InteractiveStepException::InteractiveStepException() throw() :
    PlanManagerException()
{
}


InteractiveStepException::InteractiveStepException(std::string msg) throw() :
    PlanManagerException(msg)
{
}


InteractiveStepException::InteractiveStepException(std::string msg, std::string detail) throw() :
    PlanManagerException(msg, detail)
{
}


InteractiveStepException::InteractiveStepException(std::string msg, std::string detail, std::string comment) throw() :
    PlanManagerException(msg, detail, comment)
{
}


InteractiveStepException::~InteractiveStepException() throw()
{
}
