/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveVmsRestoreScheduleStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

// namespace alias for Corba structures/types

#if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
 
namespace TA_Base_App
{

	ActiveVmsRestoreScheduleStep::ActiveVmsRestoreScheduleStep( const TA_Base_Core::StepDetail& stepDetail, 
													const TA_Base_Core::VmsRestoreScheduleParameters& params,
													IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveVmsRestoreScheduleStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = "Step executed.";
		
		std::string message;

        TA_Base_Core::NamedObject< TA_Base_App::TA_Signs::ISignFactoryCorbaDef, TA_Base_App::TA_Signs::ISignFactoryCorbaDef_ptr, TA_Base_App::TA_Signs::ISignFactoryCorbaDef_var> signFactory; 
        TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity( "VmsAgent");
        signFactory.setCorbaName( name.getAgentName(), "VMS/SignFactory" );

        try
        {
			TA_Base_App::TA_Signs::SignObjectSeq* signObjSeq ;
			CORBA_CALL_RETURN( signObjSeq,
			                   signFactory,
			                   getAll,
			                   () );
            int numberOfSigns = signObjSeq->length();
            bool foundSign = false;

            for ( int i=0; i < numberOfSigns; i++ )
            {
                TA_Base_App::TA_Signs::SignObject so = signObjSeq->operator[](i);
                if ( so.entityKey == m_params.vmsSign )
                {
                    TA_Base_App::TA_Signs::IScheduledEventOpsCorbaDef_ptr schedOps = TA_Base_App::TA_Signs::IScheduledEventOpsCorbaDef::_narrow( so.id->getOpsInterface(TA_Base_App::TA_Signs::OPS_SCHEDULEDSCENE) );
                    std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
                    schedOps->activateScheduling( session.c_str() );
					foundSign = true;
                    break;
                }
            }

            if ( !foundSign )
            {
                m_remark = "The sign was not found.";
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                return stepState;
            }

            //On successful completion of the step
            m_flowControl.setNextStep();  
            return( stepState );
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

#endif //  STEPTYPE_VMS_RESTORE_SCHEDULE
