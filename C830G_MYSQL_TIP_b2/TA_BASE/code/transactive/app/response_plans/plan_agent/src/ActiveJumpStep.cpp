/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveJumpStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_JUMP )

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveJumpStep::ActiveJumpStep( const TA_Base_Core::StepDetail& stepDetail, 
									const TA_Base_Core::JumpParameters& params,
									IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),		
		m_flowControl(flowControl)
	{
	}

		TA_Base_Core::EActiveStepState ActiveJumpStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s: jumping to step %lu", dumpContext().c_str(), m_params.step );
		m_flowControl.setNextStep( m_params.step );
			return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App


#endif //  STEPTYPE_JUMP
