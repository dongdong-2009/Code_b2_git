// ActiveNullStep.cpp: implementation of the ActiveNullStep class.
//
//////////////////////////////////////////////////////////////////////
#include "PlanAgent.h"

#include "ActiveNullStep.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
	ActiveNullStep::ActiveNullStep( const TA_Base_Core::StepDetail& stepDetail, 
									  IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_flowControl(flowControl)
	{
			m_detail.ignoreFailure = true;
			m_detail.skip = true;
			m_detail.skippable = true;
	}

	TA_Base_Core::EActiveStepState ActiveNullStep::executeImpl( void )
	{
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "should not execute the null step" );
	
		m_flowControl.setNextStep(); // Set NextStep: MP

		TA_ASSERT(false, "should not execute the null step");

		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}
}