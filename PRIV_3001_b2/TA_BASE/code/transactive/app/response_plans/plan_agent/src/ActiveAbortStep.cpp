
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_ABORT )

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveAbortStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveAbortStep::ActiveAbortStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAbortStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: aborting", dumpContext().c_str() );
		m_flowControl.setIsActive( false );

		m_remark = RemarkPlanAbort;

		ActivePlanBulletinBoard::instance()->updatePlanState( m_flowControl.getPlan(), TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE , m_remark);
		m_flowControl.setNextStep(); // Set NextStep: MP
		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App


#endif // STEPTYPE_ABORT 
