/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageBlock.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"

namespace TA_Base_App
{

ActivePlanMessageBlock::ActivePlanMessageBlock( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* data ) :
m_plan( plan ),
m_control( data )
{
}

ActivePlanMessageBlock::~ActivePlanMessageBlock(void)
{
	delete m_control;
}

IActivePlanMessage* ActivePlanMessageBlock::getControlMessage( void ) const
{
	return m_control;
}

const TA_Base_Core::ActivePlanId& ActivePlanMessageBlock::getPlan( void ) const
{
    return m_plan;
}

} // TA_Base_App
