/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTerminatePlanStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_TERMINATE_PLAN )

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveTerminatePlanStep::ActiveTerminatePlanStep( const TA_Base_Core::StepDetail& stepDetail, 
									const TA_Base_Core::TerminatePlanParameters& params,
									IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),		
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveTerminatePlanStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActiveTerminatePlanStep::executeImpl( void )" );
		bool instanceFound = false;

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s: terminating plan id %lu", dumpContext().c_str(), m_params.plan );

		std::auto_ptr<ActivePlanList> instanceList( ActivePlanBulletinBoard::instance()->getActiveInstances( m_params.plan ) );
		
		ConstActivePlanListIterator it;

        std::ostringstream reasonStrm;

        reasonStrm << "Terminated by instance " << m_flowControl.getPlan().instance << " of plan '"
                   << TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan) << "'";

        std::string reason = reasonStrm.str();

		for( it = instanceList->begin(); it != instanceList->end(); ++it )
		{
			// make sure we don't tell ourself to terminate...
			if( m_flowControl.getPlan().plan != (*it).plan )
			{
				PlanExecutor::instance()->stop( *it, reason );
				instanceFound = true;
			}
			else
			{
				if( m_flowControl.getPlan().instance != (*it).instance )
				{
					PlanExecutor::instance()->stop( *it, reason );
					instanceFound = true;
				}
			}
		}

		// Tell the active plan not to execute the next step until
		// the specified plan has terminated.
		if( instanceFound )
		{	//m_flowControl.setNextStep(); //Test setNextStep() MP
			m_flowControl.setTerminatedInterlock( m_params.plan );
		}
        else
            stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;


		m_flowControl.setNextStep();



		LOG( SOURCE_INFO, FUNCTION_EXIT_LOG_TYPE, "ActiveTerminatePlanStep::executeImpl( void )" );

		return stepState;
	}
} // TA_Base_App


#endif //  STEPTYPE_TERMINATE_PLAN
