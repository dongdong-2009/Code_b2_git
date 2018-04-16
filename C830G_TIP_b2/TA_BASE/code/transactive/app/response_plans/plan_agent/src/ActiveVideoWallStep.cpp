/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveVideoWallStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

// namespace alias for Corba structures/types

#if defined ( STEPTYPE_VIDEOWALL )
 
namespace TA_Base_App
{

	ActiveVideoWallStep::ActiveVideoWallStep( const TA_Base_Core::StepDetail& stepDetail, 
													const TA_Base_Core::VideoWallParameters& params,
													IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveVideoWallStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        TA_Base_Core::NamedObject< TA_Base_App::IVideoWallAgentCorbaDef, TA_Base_App::IVideoWallAgentCorbaDef_ptr, TA_Base_App::IVideoWallAgentCorbaDef_var> vidiWallAgent; 
        
		try
		{

            TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity( m_params.wallBoard );
            vidiWallAgent.setCorbaName( name.getAgentName(), name.getAgentName());

            std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());

			CORBA_CALL( vidiWallAgent,
			            activateLayout,
			            ( m_params.layout, 1 ) ); //TODO: get sessionid type fixed for activateLayout()

            //On successful completion of the step
            m_flowControl.setNextStep();
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what();

				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const ActiveStepParamException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what();

				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";

				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return( stepState );
	}
} // TA_Base_App

#endif //  STEPTYPE_VIDEOWALL
