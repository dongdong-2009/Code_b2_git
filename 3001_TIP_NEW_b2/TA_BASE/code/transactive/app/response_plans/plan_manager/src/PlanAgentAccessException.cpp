/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanAgentAccessException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Derived TransActive Exception, adds detail and comment members, along with related
  * accesor methods.
  */

#include "PlanAgentAccessException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


PlanAgentAccessException::PlanAgentAccessException() throw() :
    PlanManagerException()
{
}


PlanAgentAccessException::PlanAgentAccessException(std::string msg) throw() :
    PlanManagerException(msg)
{
}


PlanAgentAccessException::PlanAgentAccessException(std::string msg, std::string detail) throw() :
    PlanManagerException(msg, detail)
{
}


PlanAgentAccessException::PlanAgentAccessException(std::string msg, std::string detail, std::string comment) throw() :
    PlanManagerException(msg, detail, comment)
{
}


PlanAgentAccessException::~PlanAgentAccessException() throw()
{
}
