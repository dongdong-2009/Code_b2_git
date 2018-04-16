
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"


#if defined ( STEPTYPE_ABORT )

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveAbortStep::ActiveAbortStep( const TA_Base_Core::StepDetail& stepDetail, 
									  IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAbortStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "%s: aborting", dumpContext().c_str() );
		m_flowControl.setIsActive( false );

		m_remark = "Plan aborted.";

		ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
														 TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE , m_remark);
		m_flowControl.setNextStep(); // Set NextStep: MP
		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App


#endif // STEPTYPE_ABORT 
