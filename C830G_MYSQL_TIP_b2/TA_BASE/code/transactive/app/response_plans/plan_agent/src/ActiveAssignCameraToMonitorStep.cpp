/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "PlanAgent.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )
// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveAssignCameraToMonitorStep::ActiveAssignCameraToMonitorStep( const TA_Base_Core::StepDetail& stepDetail, 
													const TA_Base_Core::AssignCameraToMonitorParameters& params,
													IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAssignCameraToMonitorStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		try
		{
			std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
			TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationNameFromSession (session);
			TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* monitorObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutput(m_params.monitor);
			
			CORBA_CALL( (*monitorObj),
			            requestSwitchOfInput,
			            ( m_params.camera, session.c_str(), true ) );
			//On successful completion of the step
			m_flowControl.setNextStep();
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::string msg = "Error in resolving Video Switch Agent : ";
			msg += e.what();
            m_remark = msg;
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const ActiveStepParamException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what();

            m_remark = "Invalid Step Parameters: ";
            m_remark += msg.str();
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Bus::VideoSwitchAgentException& e )
		{
            m_remark = e.what.in();
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::VideoSwitchAgentException", m_remark.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";

            m_remark = "Unknown exception thrown by VideoSwitchAgent";
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return( stepState );
	}
} // TA_Base_App


#endif //  STEPTYPE_ASSIGN_CAMERA_TO_MONITOR

