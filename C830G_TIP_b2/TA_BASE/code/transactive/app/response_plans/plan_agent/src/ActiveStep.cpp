/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveStep::ActiveStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl ) :
		m_detail(stepDetail),
		m_flowControl(flowControl),
        m_delayStartTime(0), 
        m_delayCompleted(0), 
		m_GRCType3StepType(GRCTYPE3_NORMAL_STEP)
	{
	}

	void ActiveStep::customise( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
		std::ostringstream message;

		message << "ActiveStep::customise() : Plan[" << m_flowControl.getPlan().plan
			    << ":" << m_flowControl.getPlan().instance << "]" << " Step Number:" 
				<< getStepNumber();

		if ( !m_detail.skippable && newDetail.skip )
		{
			message << " Error: Skippable is not true so step cannot be skipped.";
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str() );
            TA_THROW ( TA_Base_Core::PlanAgentException (message.str(), TA_Base_Core::PAE_CANNOT_SKIP_UNSKIPPABLE));
		}

		m_detail.skip  = newDetail.skip;
		m_detail.delay = newDetail.delay;
        m_detail.ignoreFailure = newDetail.ignoreFailure;
		m_detail.nowait = newDetail.nowait;
	}

	TA_Base_Core::StepNumber ActiveStep::getStepNumber( void ) const
	{
		return m_detail.position;
	}

	void ActiveStep::load( bool fNeedUpdateBulletinBoard )
	{
		if ( m_remark.empty() )
        {
			m_remark = "Plan started";
        }

		if ( fNeedUpdateBulletinBoard )
		{
			ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
														 getStepNumber(),
														 m_detail.name.in(), m_remark );
		}
	}

    void ActiveStep::processStepExecutionFailure( const std::string& failureReason, bool isGRCType3Step )
    {
        if ( !failureReason.empty() )
        {
            m_remark = failureReason;
        }

        std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan );
		std::ostringstream logMessage;
		logMessage << "Step " << getStepNumber() << "of Plan " << planPath.c_str()
			         << " instance " << m_flowControl.getPlan().instance 
			         << " failed - " << m_remark.c_str();

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, logMessage.str().c_str());

		TA_Base_Core::DescriptionParameters descriptionParameters;
		std::string alarmId = "";
		std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());

		bool isGRCPlan = ActivePlanBulletinBoard::instance()->isGRCPlan (m_flowControl.getPlan());
		//GRC plans
		if (isGRCPlan)
		{
			
			if (ActivePlanBulletinBoard::instance()->isManualGRC (m_flowControl.getPlan()))
			{
				//TD18369++ zhaodongyang
				/*
				TA_Base_Core::NameValuePair pair( "GRC", "Executed");
				descriptionParameters.push_back ( &pair );
				*/
				
				TA_Base_Core::NameValuePair pair0( "ManualGRCPlan", "UNSUCCESSFUL");
				descriptionParameters.push_back ( &pair0 );
				TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
				descriptionParameters.push_back ( &pair1 );
				
				std::ostringstream tempStrm;
				tempStrm << m_flowControl.getPlan().instance;
				TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
				descriptionParameters.push_back ( &pair2 );
				//++TD18369 zhaodongyang
			
				//TD14802++
				//Add dummy pair to set Value column in Event Viewer to "UNSUCCESSFUL"
				TA_Base_Core::NameValuePair pair3( "DataPointValue", "UNSUCCESSFUL");
				descriptionParameters.push_back ( &pair3 );
				//++TD14802
			
				// TD18369++ zhaodongyang
				std::ostringstream stepStrm;
				stepStrm << getStepNumber();
				TA_Base_Core::NameValuePair pair4( "StepNumber", stepStrm.str());
				descriptionParameters.push_back ( &pair4 );
				TA_Base_Core::NameValuePair pair5( "Reason", m_remark);
				descriptionParameters.push_back ( &pair5 );
				// ++TD18369 zhaodongyang
			
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanManualGRCUnsuccessful, descriptionParameters);	
			}
			else//automatic GRC
			{
				//TD18369++ zhaodongyang
				/*
				TA_Base_Core::NameValuePair pair( "GRC", "Executed");
				descriptionParameters.push_back ( &pair );
				*/
				
				TA_Base_Core::NameValuePair pair0( "AutoGRCPlan", "UNSUCCESSFUL");
				descriptionParameters.push_back ( &pair0 );
				TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
				descriptionParameters.push_back ( &pair1 );
				
				std::ostringstream tempStrm;
				tempStrm << m_flowControl.getPlan().instance;
				TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
				descriptionParameters.push_back ( &pair2 );
				//++TD18369 zhaodongyang

				//TD14802++
				//Add dummy pair to set Value column in Event Viewer to "UNSUCCESSFUL"
				TA_Base_Core::NameValuePair pair3( "DataPointValue", "UNSUCCESSFUL");
				descriptionParameters.push_back ( &pair3 );
				//++TD14802
				
				//TD18369++ zhaodongyang
				std::ostringstream stepStrm;
				stepStrm << getStepNumber();
				TA_Base_Core::NameValuePair pair4( "StepNumber", stepStrm.str());
				descriptionParameters.push_back ( &pair4 );
				TA_Base_Core::NameValuePair pair5( "Reason", m_remark);
				descriptionParameters.push_back ( &pair5);
				// ++TD18369 zhaodongyang
			
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanAutoGRCUnsuccessful, descriptionParameters);	
			}
		}
		//TD14803 azenitha++ / wenguang++ 
		//For GRC, alarm is not needed
		else 
		{
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "UNSUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927
			
			TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
			descriptionParameters.push_back ( &pair1 );
			
			std::ostringstream tempStrm;
			tempStrm << m_flowControl.getPlan().instance;
			TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
			descriptionParameters.push_back ( &pair2 );
			std::ostringstream stepStrm;
			stepStrm << getStepNumber();
			TA_Base_Core::NameValuePair pair3( "StepNumber", stepStrm.str());
			descriptionParameters.push_back ( &pair3 );
			TA_Base_Core::NameValuePair pair4( "Reason", m_remark);
			descriptionParameters.push_back ( &pair4 );
			

			if (ActivePlanBulletinBoard::instance()->isManualPlan (m_flowControl.getPlan()))
			{
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanStepExecFailure, descriptionParameters);
				//throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_PLAN_EXEC_ERROR, 0, "Could not execute plan step"));
			}
			else
			{
				alarmId = AAMessageSender::instance()->raiseAlarm(
							 TA_Base_Core::PlanAgentAlarms::PlanStepExecutionFailure, descriptionParameters);
				ActivePlanBulletinBoard::instance()->setAssociatedAlarmId (m_flowControl.getPlan(), alarmId.c_str ());
			}
		}
		//TD14803 ++azenitha / ++wenguang

		m_flowControl.setInteractiveStep(false);
		
		if ( m_detail.ignoreFailure )
		{
            // TD8788: Where a step failure does NOT cause the plan to pause,
            // the associated alarm remains open until it is manually closed
            // by the operator. ActivePlan is not given the alarm ID.

            m_flowControl.setPlanStepFailed();
		    ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
											     getStepNumber(), TA_Base_Core::FAILED_ACTIVE_STEP_STATE, m_remark );
			//TD20350
            if ( ! isGRCType3Step )
			{
				m_flowControl.setNextStep();
				//lizettejl++ (DP-changes)pass in the step number
				PlanExecutor::instance()->resume( m_flowControl.getPlan(), getStepNumber());
				//++lizettejl
			}

        }
        else
        {
            // TD8788: Pass on the alarm ID to allow the alarm to be closed
            // when the plan is resumed or stopped.
			
			//TD20350
			TA_ASSERT(!isGRCType3Step, "the step should not be GRC Type 3 Step");

            m_flowControl.pausePlan( m_remark, alarmId );
            ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
                                                getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, m_remark);
        }
    }

	void ActiveStep::execute( void )
	{
		bool isGRCType3Step = ( m_GRCType3StepType != GRCTYPE3_NORMAL_STEP );
		bool isGRCType3ParentStep = ( m_GRCType3StepType == GRCTYPE3_PARENT_STEP );
		bool isGRCType3ChildStep = ( m_GRCType3StepType == GRCTYPE3_CHILD_STEP );

		if( m_detail.skippable && m_detail.skip )
		{
			TA_ASSERT(!isGRCType3Step, "this step should not be GRC Type 3 Step"); 

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s: skip.", dumpContext().c_str() );

            load();

            if ( m_remark.empty() )
            {
				m_remark = "Step skipped";
            }

			ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(), 
                getStepNumber(), TA_Base_Core::SKIPPED_ACTIVE_STEP_STATE, m_remark );
            m_flowControl.setNextStep();    //MP: skip to the next step
			return;
		}

        try
        {
            if (m_detail.delay > 0)
            {
                // Get the current state only delay is enable
                TA_Base_Core::EActivePlanState state = 
                    ActivePlanBulletinBoard::instance()->getState( m_flowControl.getPlan() );

                if ( state == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE )
                {
                    load(!isGRCType3ParentStep);

                    // Check if we are partway through a delay.
                    if (m_delayCompleted != 0)
                    {
                        // Part way through a delay. Update the start time so that the total delay is correct.
                        m_delayStartTime = time(NULL) - m_delayCompleted;
                    }
                    else
                    {
                        // Keep a timestamp of the time the delay started.
                        m_delayStartTime = time(NULL);
                    }

                    // Update the remark to indicate we're in a delay
                    std::ostringstream remark;
                    remark << "In a " << m_detail.delay << " second delay";
                    m_remark = remark.str();

				    if ( ! isGRCType3ParentStep )
				    {
					    ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
                            TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE, m_remark);

					    ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
                            getStepNumber(), TA_Base_Core::DELAYED_ACTIVE_STEP_STATE, m_remark);
				    }

                    // Update the state of the plan to delayed.
                    m_flowControl.changeState(ActivePlanDelayedState::instance());
                    state = TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE;
                }

                if (state == TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE)
                {
                    if ( 0 == m_delayStartTime )
                    {
                        // The state is restored from monitor mode, simply re-wait
                        m_delayStartTime = time(NULL);
                        return;
                    }

                    // Check that the delay hasn't completed yet.
                    time_t currentTime = time(NULL);
                    if (m_delayStartTime + m_detail.delay > currentTime)
                    {
                        // Delay not complete. Update the amount of the delay completed and return.
                        m_delayCompleted = time(NULL) - m_delayStartTime;
                        return;
                    }

                    // TD10841: Set active plan state back to executing after the delay has completed.
                    m_flowControl.changeState(ActivePlanExecutingState::instance());
                    state = TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE;
                }
            }
            else if (m_detail.delay == 0)
            {
                //Introduce a short finite delay if the delay is zero
			    ACE_Time_Value tv( 0, 50000 ); // 50ms
			    ACE_OS::sleep( tv );
            }
        }
        catch ( ... )
        {
            // Do nothing as plan state was not found, let the processing 
            // be default
        }

        // Update the remark in case it currently indicates that we're in a delay.
        m_remark = "Executing step";

        load(!isGRCType3ParentStep);

        // Delay is finished. Reset the amount of the delay completed.
        m_delayCompleted = 0;

		if ( ! isGRCType3ParentStep )
		{
			ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
                TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, m_remark);
		}

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;

		try
		{
			stepState = executeImpl();
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "unknown", dumpContext().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		//TD20350
		if( TA_Base_Core::FAILED_ACTIVE_STEP_STATE == stepState  )
		{
			//should not process GRCType3ParentStep failure
			if ( ! isGRCType3ParentStep )
			{
				processStepExecutionFailure("", isGRCType3Step);
			}
		}
		else
		{
			if ( m_remark.empty() )
				m_remark = "Plan step executed";

			if ( ! isGRCType3ParentStep )
			{
				ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
														 getStepNumber(),
														 stepState, m_remark );
			}
		}
	}

	std::string ActiveStep::dumpContext( void ) const
	{
		std::ostringstream context;

		context << "[P:"
				<< m_flowControl.getPlan().plan
				<< ",I:"
				<< m_flowControl.getPlan().instance
				<< ",S:"
				<< getStepNumber()
				<< "]";

		return( context.str() );
	}

} // TA_Base_App
