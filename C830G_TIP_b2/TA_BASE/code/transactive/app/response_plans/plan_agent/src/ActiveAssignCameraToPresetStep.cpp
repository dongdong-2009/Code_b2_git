/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAssignCameraToPresetStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )
// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveAssignCameraToPresetStep::ActiveAssignCameraToPresetStep( const TA_Base_Core::StepDetail& stepDetail, 
													const TA_Base_Core::ActivateCameraPresetParameters& params,
													IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAssignCameraToPresetStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
        bool cameraLocked = false;
        VideoSwitchAgentFactory::VideoInputNamedObject* videoInputObject = NULL;
		try
		{
            videoInputObject = VideoSwitchAgentFactory::getInstance().getVideoInput( m_params.camera );
			CORBA_CALL( (*videoInputObject),
			            lock,
			            ( session.c_str() ) );
            cameraLocked = true;
        }
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::string msg = "Error in resolving Video Switch Agent : ";
			msg += e.what();
            m_remark = msg;
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.c_str() );
		}
        catch ( const TA_Base_App::VideoSwitchAgentException & e )
        {
            m_remark = e.what.in();
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::VideoSwitchAgentException", m_remark.c_str() );
        }
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";
            m_remark = "Could not get lock on Camera";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
		}

        if (!cameraLocked)
        {
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		    return( stepState );
        }

		try
		{
            VideoSwitchAgentFactory::CameraNamedObject* cameraObj = VideoSwitchAgentFactory::getInstance().getCamera(m_params.camera);

            std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
			CORBA_CALL( ((*cameraObj)->getMovementControl()),
			            activatePreset,
			            ( m_params.preset, session.c_str() ) );
            //On successful completion of the step
            delete cameraObj;
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

            m_remark = msg.str();
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", m_remark.c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
        catch ( const TA_Base_App::VideoSwitchAgentException & e )
        {
            m_remark = e.what.in();
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::VideoSwitchAgentException", m_remark.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";

            m_remark = "UnknownException thrown by Video Switch Agent";

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception after getting lock on Camera", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

        if ( !cameraLocked )
            return( stepState );
        try
        {
            if ( videoInputObject != NULL )
            {
                (*videoInputObject)->unlock(session.c_str());
                delete videoInputObject;

                cameraLocked = false;
            }
        }
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::string msg = "Error in resolving Video Switch Agent : ";
			msg += e.what();
            m_remark = msg;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException, could not unlock camera", msg.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
        catch ( const TA_Base_App::VideoSwitchAgentException & e )
        {
            m_remark = e.what.in();
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::VideoSwitchAgentException", m_remark.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";
            m_remark = "Could not unlock Camera";

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception : Could not unlock Camera", msg.str().c_str() );
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}


        if ( cameraLocked )
        {
            return( stepState );
        }
        //On successful completion of the step
        m_flowControl.setNextStep();

		return( stepState );
	}

} // TA_Base_App

#endif //  STEPTYPE_ACTIVATE_CAMERA_PRESET

