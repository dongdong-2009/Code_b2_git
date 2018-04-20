/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlan.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #6 $
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
	ActivePlan::ActivePlan( const TA_Base_Core::ActivePlanId& plan, 
        TA_Base_Core::EActivePlanState initialState, const bool bStateSyncDone ) :
        // Initialize all member variables correctly
        m_plan(plan),
        m_state( NULL ),
		m_isActive( true ),
        m_remark( "" ),
        m_steps(),
        m_stepIterator(),
		m_terminatedInterlockIsSet( false ),
		m_completedInterlockIsSet( false ),
        m_operatorResponseInterlockIsSet( false ),
        m_hasPlanCompleted( false ),
        m_stepFailed( false ),
        m_failedStepAlarmId( "" ),
		m_terminatedInterlock( 0 ),
        m_completedInterlock(),
		m_resultTimeout( 0, 10000 ),
        m_lastExecutedStep(),
        m_newSessionId( "" ),
        m_newPmId( "" ),
        m_isTakeControlPending ( false ),
        m_isPausePending ( false ),
        m_isCurrentInteractiveStep( false ),
        m_hasInteractiveStepFailed( false ),
        m_bStateSyncDone( bStateSyncDone )
	{
		m_completedInterlock.plan     = 0;
		m_completedInterlock.instance = 0;

		std::ostringstream message;


		switch( initialState )
		{
				case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
				m_state = ActivePlanExecutingState::instance();
				break;

				case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
				m_state = ActivePlanPendingState::instance();
				break;

			default:
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "%s: Invalid initial state, defaulting to \"Executing\"", dumpContext().c_str() );
				m_state = ActivePlanExecutingState::instance();
				break;
		}
		
        std::string reason = "";
		std::auto_ptr<ActiveStepList> tmpList( PlanAccessFactory::instance()->getActiveStepList( m_plan.plan, *this, reason  ));

        if ( !reason.empty() )
        {
            TA_THROW( PlanNotFoundException(reason, TA_Base_Core::PNFE_FAIL_FETCH_ACTIVE_LIST_STEP, plan.plan, plan.instance));
        }

		m_steps = tmpList;

		if( NULL == m_steps.get() || (0 == m_steps->size()) )
		{
			std::ostringstream what;
			what << dumpContext() << ": failed to get the active step list";
			message << "Active Plan: Failed  to get the active step list for plan[" << plan.plan
				    << ":" << plan.instance << "]";
            TA_THROW(PlanNotFoundException(message.str(), TA_Base_Core::PNFE_FAIL_FETCH_ACTIVE_LIST_STEP, plan.plan, plan.instance));
		}
		else
		{
			m_stepIterator = m_steps->begin();
		}
	}

	ActivePlan::~ActivePlan()
	{
		// It would have been nice to be able to use a "list of auto_ptr" but
		// the g++ implementation of the STL didn't allow it. So instead we
		// have to delete all the individual steps. As the list itself is an
		// auto_ptr we don't have to delete that here.
		ActiveStepListIterator it;

		for( it = m_steps->begin(); it != m_steps->end(); ++it )
		{
			delete (*it);
		}
	}

	ActivePlan::ENextStepExecuteMethod ActivePlan::executeNextStep( void )
	{
        //The next step of a Plan is executed only if the PlanAgent is in Control mode
        // and not in Monitor mode. And Plan state synchronization from Control mode is done.

        if ( !m_bStateSyncDone || !ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            return NORMAL_NSEM;
        }

        return m_state->executeNextStep( *this );
	}

    void ActivePlan::stop( std::string& reason )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::stop( void )" );
		m_state->stop( *this, reason );

        // TD8788: The alarm ID is set when the plan pauses due to step failure.
        // Where the plan is subsequently stopped, the alarm should be cleared.
        if (!m_failedStepAlarmId.empty())
        {
            AAMessageSender::instance()->closeAlarm(m_failedStepAlarmId);
            m_failedStepAlarmId = "";
        }

		TA_Base_Core::DescriptionParameters descriptionParameters;
		
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927

        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		descriptionParameters.push_back ( &pair1 );

        std::ostringstream tempStrm;
        tempStrm << getPlan().instance;
        TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		descriptionParameters.push_back ( &pair2 );

		std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());
		std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId(getPlan());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanStop, descriptionParameters);

        //Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = CORBA::string_dup(session.c_str());
		psu->pmId = CORBA::string_dup(pmid.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		psu->plan = getPlan();
        psu->remark = CORBA::string_dup(m_remark.c_str());
		pasUpdateData->update.stopPlan(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
	}

    void ActivePlan::complete( void )
    {
        m_state->complete ( *this );
    }

	void ActivePlan::pause( void )
	{
        transferControl();  //This transfers control if pending

		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::pause( void )" );
		m_state->pause( *this ); 
        
        if (!m_isCurrentInteractiveStep)
        {
            m_isPausePending = false;

			if (m_remark.empty ())
			{
				m_remark = "Plan paused by operator";
			}

   		   ActivePlanBulletinBoard::instance()->update( getPlan(), getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, m_remark );
        }

		TA_Base_Core::DescriptionParameters descriptionParameters;
		
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927

        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		descriptionParameters.push_back ( &pair1 );

        std::ostringstream tempStrm;
        tempStrm << getPlan().instance;
        TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		descriptionParameters.push_back ( &pair2 );

		std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());
		std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId(getPlan());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		//send this message for DSS plans only
		if (!ActivePlanBulletinBoard::instance()->isGRCPlan (getPlan()))
		{
			AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanPause, descriptionParameters);
		}
        
        //Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = CORBA::string_dup(session.c_str());
		psu->pmId = CORBA::string_dup(pmid.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		psu->plan = getPlan();
		pasUpdateData->update.pausePlan(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
	}

	void ActivePlan::pausePlan ( const std::string& remark, const std::string& failedStepAlarmId )
	{
        m_remark = remark;
        m_failedStepAlarmId = failedStepAlarmId;

        pause();
	}

	void ActivePlan::pauseForOperator( void )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::pauseForOperator( void )" );
		m_state->pauseForOperator( *this );
	}

	//lizettejl++ 
	//DP-changes : modified to include the step number where the plan should resume execution
	void ActivePlan::resume( TA_Base_Core::StepNumber resumeStepPosition )
	{
		// resuming from an interactive step always passes a resumeStepPosition
		// which is 1 step less than the current step number, so  
		// (resumeStepPosition != getStepNumber()) does not determine that the step is not
		// from a current position. 
		// note: resumeStepPosition 
		//if (resumeStepPosition != getStepNumber())
		//{
		//	 broadcast plan update with step state = UNEXECUTED_ACTIVE_STEP_STATE
		//	m_remark = "The current step has transitioned from paused state to unexecuted state.";
		//	ActivePlanBulletinBoard::instance()->update( getPlan(),resumeStepPosition, 
		//	TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE, m_remark );
		//	
		//	setCurrentStepPosition(resumeStepPosition);
        //}

		if ( m_hasInteractiveStepFailed )
		{
			m_hasInteractiveStepFailed = false;
			m_isPausePending = false;
			setOperatorResponseInterlock(false);
		}
		else if ( m_isPausePending )
		{
			m_isPausePending = false;
			setOperatorResponseInterlock(false);
			
			if ( m_remark.empty() )
				m_remark = "Plan paused by operator";
			
			ActivePlanBulletinBoard::instance()->update( getPlan(), getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, m_remark );				
			
			return;
		}			
		
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::resume( void )" );

		//m_operatorResponseInterlock = false;            
		m_state->resume( *this );
		
		// TD8788: The alarm ID is set when the plan pauses due to step failure.
		// Where the plan is subsequently resumed, the alarm should be cleared.
		if (!m_failedStepAlarmId.empty())
		{
			AAMessageSender::instance()->closeAlarm(m_failedStepAlarmId);
			m_failedStepAlarmId = "";
		}

		std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());
		std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId(getPlan());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 

		//Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
		
		pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		
		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = CORBA::string_dup(session.c_str());
		psu->pmId = CORBA::string_dup(pmid.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		psu->plan = getPlan();
		pasUpdateData->update.resumePlan(psu);
		ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
	}

	void ActivePlan::reassign( const std::string& sessionId, const std::string& pmId )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::reassign( std::string sessionId )" );


		m_state->reassign( *this, sessionId, pmId );
	}

	void ActivePlan::unassign( void )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::unassign( void )" );
		m_state->unassign( *this );
	}

	void ActivePlan::customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
		LOG( SOURCE_INFO, FUNCTION_ENTRY_LOG_TYPE, "ActivePlan::customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail )" );
		m_state->customiseStep( *this, newDetail );
    }

	std::string ActivePlan::getRemark( void )
	{
		return m_remark;
	}

    void ActivePlan::scheduleTakeControl( const std::string session, const std::string& pmId )
    {
        m_isTakeControlPending = true;
        m_newSessionId = session;
        m_newPmId = pmId;
    }

    void ActivePlan::schedulePause()
    {
        m_isPausePending = true;
    }

    void ActivePlan::transferControl(void)
    {
        if ( !m_isTakeControlPending ) return;

        reassignImpl( m_newSessionId, m_newPmId );
        m_isTakeControlPending = false;
    }

    void ActivePlan::setCurrentStepPosition( TA_Base_Core::StepNumber stepNumber )
    {
        //Set the current step position
		if( (m_steps->end() == m_stepIterator) ||
			(stepNumber < (*m_stepIterator)->getStepNumber()) )
        {
            // If stepNum = 0, stepNumber - 1 will be -1, list iterator out will out of boundary
            if ( 0 < stepNumber )
            {
			    m_stepIterator = m_steps->begin();
			    std::advance( m_stepIterator, stepNumber - 1 );
            }
            else
            {
                // stepNumber is 0, keep m_stepIterator = m_steps->end()
            }
		}
		else
		{
			std::advance( m_stepIterator, (stepNumber - (*m_stepIterator)->getStepNumber()) );
		}

		if( m_steps->end() != m_stepIterator )
		{
			TA_ASSERT( stepNumber == (*m_stepIterator)->getStepNumber() , "Failed to set new step number." );
		}

		if (m_steps->size() < stepNumber)
		{
			m_hasPlanCompleted = true;
		}
        else
        {
            if ( m_isPausePending )
                pause();            
        }
    }


	void ActivePlan::setNextStep( TA_Base_Core::StepId step )
	{

		TA_Base_Core::StepNumber stepNumber = TA_Base_Bus::PlanDataReader::instance()->getStepNumber(step);

        setCurrentStepPosition(stepNumber);

	}

	void ActivePlan::publishNextStepDetail( void ) const
	{
		if( m_steps->end() != m_stepIterator )
		{
			(*m_stepIterator)->load();
		}
	}

	void ActivePlan::setNextStep( void )
	{
		m_stepIterator++;

        // This is a problem. current step is changed but Monitor Agent doesn't know this
        // when paused, Monitor Agent will be paused at the old step. 
        // although the other following update message can update the current step in Monitor Agent,
        // The execution history can't be updated and remain pause state for the old step.
        // Consider to send step position update message to Monitor Agent
		if( m_steps->end() != m_stepIterator )
		{
            if ( m_isPausePending )
            {
                pause();
            }
            
            return;
        }

		m_hasPlanCompleted = true;
		setIsActive( false );

        if ( anyPlanStepFailed() )
		    m_remark = "Plan completed with failed step(s)";
        else
		    m_remark = "Plan completed successfully";

		ActivePlanBulletinBoard::instance()->update( getPlan(),
														 TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );

		TA_Base_Core::DescriptionParameters descriptionParameters;

		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927

        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		descriptionParameters.push_back ( &pair1 );

        std::ostringstream tempStrm;
        tempStrm << getPlan().instance;
        TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		descriptionParameters.push_back ( &pair2 );

		std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());

		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanComplete, descriptionParameters);

		std::ostringstream auditMessage;
		TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(getPlan().plan);

		auditMessage << TA_Base_Bus::PlanDataReader::instance()->getNodeName(getPlan().plan).c_str()
					 << "[Id:"
					 << getPlan().plan
					 << ",Instance:"
					 << getPlan().instance
					 << "] in Category[" 
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str()
					 << "]. has completed.";

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());

	}

	void ActivePlan::setTerminatedInterlock( TA_Base_Core::NodeId interlock )
	{
		m_terminatedInterlock = interlock;
		m_terminatedInterlockIsSet = true;
	}

	void ActivePlan::setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock )
	{
		m_completedInterlock = interlock;
		m_completedInterlockIsSet = true;
	}

	void ActivePlan::setOperatorResponseInterlock( bool interlock )
	{
		m_operatorResponseInterlockIsSet = interlock;
	}

    ActiveStep* ActivePlan::getCurrentStep()
    {
        if (m_steps->end() == m_stepIterator)
            return NULL;


        return *m_stepIterator;            
    }

	TA_Base_Core::StepNumber ActivePlan::getStepNumber()
	{

		return (*m_stepIterator)->getStepNumber();
	}

	bool ActivePlan::interlockOk( void )
	{
		bool rval = true;
		EWaitResult result = UNDEFINED_WR;

		if( m_terminatedInterlockIsSet )
		{
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + m_resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForTermination( m_terminatedInterlock, absTime );

			if( SUCCESS_WR == result )
			{
				m_terminatedInterlockIsSet = false;
			}
		}
		if( m_completedInterlockIsSet )
		{
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + m_resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForCompletion( m_completedInterlock, absTime );

			if( SUCCESS_WR == result )
			{
				m_completedInterlockIsSet = false;
			}
		}
		
        if (m_operatorResponseInterlockIsSet)
        {
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + m_resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForOperatorResponse( getPlan(), absTime );

			if( SUCCESS_WR == result )
			{
				m_operatorResponseInterlockIsSet = false;
			}
        }


		// Note that we have to set the status for the interlocked steps here.
		switch( result )
		{
			case TIMEOUT_WR:
			{
				rval = false;
			}
			break;

			case SUCCESS_WR:
			{
				m_remark = "Step executed.";
				ActivePlanBulletinBoard::instance()->update( getPlan(), m_lastExecutedStep, TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, m_remark );
				rval = true;
			}
			break;

			case FAILURE_WR:
			{
				m_remark = "Step execution failed";
				ActivePlanBulletinBoard::instance()->update( getPlan(), m_lastExecutedStep, TA_Base_Core::FAILED_ACTIVE_STEP_STATE, m_remark );
				ActivePlanBulletinBoard::instance()->update( getPlan(), TA_Base_Core::FAILED_ACTIVE_PLAN_STATE, m_remark );
				setIsActive( false );
				rval = false;
			}
			break;

			default:
			// There was no interlocking.
			break;
		}

		return( rval );
    }

    // Synchronize customise steps date from Control Agent to Monitor Agent
    void ActivePlan::customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails )
    {
        for ( int nLoop = 0; nLoop < newDetails.length(); ++nLoop )
        {
            if (( newDetails[nLoop].position > m_steps->size() ) || ( newDetails[nLoop].position <= 0 ))
            {
                continue;
            }
            
            ActiveStepListIterator it;
            it = m_steps->begin();
            std::advance( it, newDetails[nLoop].position - 1 );
            
		    int stepNumber = (*it)->getStepNumber();

            if( m_steps->end() != it )
            {
                TA_ASSERT( stepNumber == newDetails[nLoop].position, "Failed to set step number." );

                (*it)->customise( newDetails[nLoop] );
            }
        }
    }

	void ActivePlan::customiseStepImpl( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
		ActiveStepListIterator it;

		it = m_steps->begin();
		std::advance( it, newDetail.position - 1 );

		int stepNumber = (*it)->getStepNumber();

		if( m_steps->end() != it )
		{
			TA_ASSERT( stepNumber == newDetail.position, "Failed to set step number." );
			(*it)->customise( newDetail );

			ActivePlanBulletinBoard::instance()->update(getPlan(), newDetail );	
			
		    TA_Base_Core::DescriptionParameters descriptionParameters;
		    
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927

            TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		    descriptionParameters.push_back ( &pair1 );

            std::ostringstream tempStrm;
            tempStrm << getPlan().instance;
            TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		    descriptionParameters.push_back ( &pair2 );

            std::ostringstream stepStrm;
            stepStrm << getStepNumber();
            TA_Base_Core::NameValuePair pair3( "StepNumber", stepStrm.str());
		    descriptionParameters.push_back ( &pair3 );

		    std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());

		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanStepCustomisation, descriptionParameters);


            //Send State Update message for this customisation
		    TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

            pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		    TA_Base_Core::PlanCustomiseStepUpdate_var customiseStepUpdate = new TA_Base_Core::PlanCustomiseStepUpdate;
		    customiseStepUpdate->session = CORBA::string_dup(session.c_str());
		    customiseStepUpdate->plan = getPlan();
            customiseStepUpdate->detail = newDetail;
		    pasUpdateData->update.customisedStep(customiseStepUpdate);
            ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
		}
	}

	void ActivePlan::reassignImpl( const std::string& sessionId, const std::string& pmId )
	{
        std::string operatorName = ActivePlanBulletinBoard::instance()->getOperatorName( getPlan() );
		ActivePlanBulletinBoard::instance()->update( getPlan(), sessionId, pmId );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927

        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		descriptionParameters.push_back ( &pair1 );

        std::ostringstream tempStrm;
        tempStrm << getPlan().instance;
        TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		descriptionParameters.push_back ( &pair2 );

        TA_Base_Core::NameValuePair pair3( "OperatorName", operatorName);
		descriptionParameters.push_back ( &pair3 );

		std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());

		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanControlAcquire, descriptionParameters);

        //Send State Update message for Taking control
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = CORBA::string_dup(sessionId.c_str());
        psu->pmId = CORBA::string_dup(pmId.c_str());
		psu->plan = getPlan();
		pasUpdateData->update.takeControl(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
	}


	ActivePlan::ENextStepExecuteMethod ActivePlan::executeNextStepImpl( void )
	{
		ENextStepExecuteMethod rval = UNDEFINED_NSEM;

		if ( !getIsActive() ) return rval;

		if( m_steps->end() == m_stepIterator )
		{
			rval = NORMAL_NSEM;
		}
		else
		{
			// Perform any interlocking.
			if( interlockOk() )
			{
                transferControl();  //This transfers control if pending
				// Record the execution of this step.
                TA_Base_Core::StepNumber stepNumber = (*m_stepIterator)->getStepNumber();

				m_lastExecutedStep = (*m_stepIterator)->getStepNumber() ;

				(*m_stepIterator)->execute();

                //The next step to be set up will be set up by the step itself and
                //not set here in the ActivePlan. //MP 30 July 2003

				// Get ready to execute the next step.
				//setNextStep(); 

				if( m_terminatedInterlockIsSet || m_completedInterlockIsSet )
				{
					rval = INTERLOCKED_NSEM;
				}
				else
				{
					rval = NORMAL_NSEM;
				}
			}
			else
			{
				//LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "%s: Not executing next step due to interlocking.", dumpContext().c_str() );
				rval = INTERLOCKED_NSEM;
			}

			if ( m_hasPlanCompleted )
			{
				setIsActive( false );

                if ( anyPlanStepFailed() )
		            m_remark = "Plan completed with failed step(s)";
                else
		            m_remark = "Plan completed successfully";

				ActivePlanBulletinBoard::instance()->update( getPlan(),
																 TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );

		        TA_Base_Core::DescriptionParameters descriptionParameters;

				//xufeng++ TD14927
				TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
				descriptionParameters.push_back ( &pair0 );
				//++xufeng TD14927

                TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(getPlan().plan ));
		        descriptionParameters.push_back ( &pair1 );

                std::ostringstream tempStrm;
                tempStrm << getPlan().instance;
                TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		        descriptionParameters.push_back ( &pair2 );

		        std::string session = ActivePlanBulletinBoard::instance()->getSession(getPlan());

		        AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanComplete, descriptionParameters);
			}
		}

		return( rval );
	}

	void ActivePlan::changeState(ActivePlanState* state)
	{
		m_state = state;
	}

	std::string ActivePlan::dumpContext( void )
	{
		std::ostringstream context;

		context << "[P:"
				<< m_plan.plan
				<< ",I:"
				<< m_plan.instance
				<< "]";

		return( context.str() );
	}

    void ActivePlan::setStateSyncDone( bool bSyncDone )
    {
        m_bStateSyncDone = bSyncDone;
    }
} // TA_Base_App
