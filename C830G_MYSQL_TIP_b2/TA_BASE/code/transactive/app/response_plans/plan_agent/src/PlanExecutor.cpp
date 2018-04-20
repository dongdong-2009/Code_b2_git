/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutor.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "PlanAgent.h"


namespace TA_Base_App
{
	PlanExecutorImpl::PlanExecutorImpl() :
		m_executionEngine(new PlanExecutionEngine)
	{
			if( -1 == m_executionEngine->open(0) )
			{
				std::string message = "Active plan thread pool could not be created.";
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
                TA_THROW( TA_Base_Core::PlanAgentException( message.c_str(), TA_Base_Core::PAE_INITIALISATION_FAILURE ) );
			}
	}

	PlanExecutorImpl::~PlanExecutorImpl()
	{
	}

	void PlanExecutorImpl::waitForRunningPlans( void )
	{
		if( NULL != m_executionEngine.get() )
		{
            // jian.huang use terminatePlanExecutionEngine instead of ACE_Message_Block::MB_HANGUP
            // to notify idle threads
// 			TA_Base_Core::ActivePlanId plan;
// 			plan.plan = 0;
// 			plan.instance = 0;
// 
// 			ActivePlanMessageBlock* msg = new ActivePlanMessageBlock(plan, NULL);

// 			msg->msg_type( ACE_Message_Block::MB_HANGUP );
// 			if( -1 != m_executionEngine->put(msg) )
// 			{
// 				m_executionEngine->wait();
// 			}
            m_executionEngine->terminateAndWait();
		}
	}

    void PlanExecutorImpl::processDecisionStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status)
    {
#if defined ( STEPTYPE_DECISION )
        ActivePlanDecisionStepMessage* decisionStepMessage = new ActivePlanDecisionStepMessage(status);
        sendMessage( plan, decisionStepMessage );
#endif //STEPTYPE_DECISION

    }

    void PlanExecutorImpl::processPromptStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status)
    {
        ActivePlanPromptStepMessage* promptStepMessage = new ActivePlanPromptStepMessage(status);
        sendMessage( plan, promptStepMessage );
    }

    void PlanExecutorImpl::processLogStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& logMessage)
    {
        ActivePlanLogStepMessage* logStepMessage = new ActivePlanLogStepMessage(status, logMessage);
        sendMessage( plan, logStepMessage );
    }

    void PlanExecutorImpl::processRunPlanStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
    {
        ActivePlanRunPlanStepMessage* runPlanStepMessage = new ActivePlanRunPlanStepMessage(status, error);
        sendMessage( plan, runPlanStepMessage );
    }

    void PlanExecutorImpl::processLaunchGuiAppStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
    {
        ActivePlanLaunchGuiAppStepMessage* launchGuiAppStepMessage = new ActivePlanLaunchGuiAppStepMessage(status, error);
        sendMessage( plan, launchGuiAppStepMessage );
    }

    void PlanExecutorImpl::processLaunchSchematicStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
    {
        ActivePlanLaunchSchematicStepMessage* launchSchematicStepMessage = new ActivePlanLaunchSchematicStepMessage(status, error);
        sendMessage( plan, launchSchematicStepMessage );
    }

	//lizettejl++ (DP-Changes)
	void PlanExecutorImpl::processFailureSummaryStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
    {
		ActivePlanFailureSummaryStepMessage* failureSummaryStepMessage = new ActivePlanFailureSummaryStepMessage(status,error);        
        sendMessage( plan, failureSummaryStepMessage );
    }
	//++lizettejl

	void PlanExecutorImpl::run(const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool manExecution, const bool bStartFromMonitorMode )
	{
		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		std::ostringstream message;

		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( plan.plan );
		}
		catch (TA_Base_Core::DataException de)
		{
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] not found.";
            TA_THROW(PlanNotFoundException(message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, plan.plan, plan.instance));
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

		m_executionEngine->run( session, pmId, plan, TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE , manExecution, bStartFromMonitorMode );
	}

	void PlanExecutorImpl::customise(const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool bStartFromMonitorMode )
	{
		std::ostringstream message;

		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( plan.plan );
		}
		catch (TA_Base_Core::DataException de)
		{
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] not found.";
            TA_THROW( PlanNotFoundException( message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, plan.plan, plan.instance) );
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

        // manExecution always true here as customisation can only be done manually
		m_executionEngine->run( session, pmId, plan, TA_Base_Core::PENDING_ACTIVE_PLAN_STATE, true, bStartFromMonitorMode );
	}

	void PlanExecutorImpl::spawnPending(const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bStartFromMonitorMode )
	{
		std::ostringstream message;

		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( childPlan.plan );
		}
		catch (TA_Base_Core::DataException de)
		{
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] not found.";
            TA_THROW( PlanNotFoundException( message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, childPlan.plan, childPlan.instance) );
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

		m_executionEngine->spawn( session, pmId, parentPlan, childPlan, TA_Base_Core::PENDING_ACTIVE_PLAN_STATE, bStartFromMonitorMode );
	}

    void PlanExecutorImpl::stop(const TA_Base_Core::ActivePlanId& plan, std::string& reason)
	{
		ActivePlanStopMessage* stopMessage = new ActivePlanStopMessage( reason );
		sendMessage( plan, stopMessage );
	}

	void PlanExecutorImpl::complete(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanCompleteMessage* completeMessage = new ActivePlanCompleteMessage;
		sendMessage( plan, completeMessage );
	}

	void PlanExecutorImpl::pause(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanPauseMessage* pauseMessage = new ActivePlanPauseMessage;
		sendMessage( plan, pauseMessage );
	}

	void PlanExecutorImpl::pauseForOperator(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanPauseForOperatorMessage* pauseForOperatorMessage = new ActivePlanPauseForOperatorMessage;
		sendMessage( plan, pauseForOperatorMessage );
	}

	//lizettejl++ update class to include the resumeStep (DP-changes)
	void PlanExecutorImpl::resume(const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepNumber resumeStepPosition)
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "PlanExecutor::instance()->resume() = %lu", resumeStepPosition);
		ActivePlanResumeMessage* resumeMessage = new ActivePlanResumeMessage(resumeStepPosition);
		sendMessage( plan, resumeMessage );
	}
	//++lizettejl


	void PlanExecutorImpl::reassign(const std::string& session, const std::string& planManagerId, const TA_Base_Core::ActivePlanId& plan )
	{
		ActivePlanReassignMessage* reassignMessage = new ActivePlanReassignMessage(session, planManagerId);
		sendMessage( plan, reassignMessage );
	}

	void PlanExecutorImpl::unassign(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanUnassignMessage* unassignMessage = new ActivePlanUnassignMessage;
		sendMessage( plan, unassignMessage );
	}

	void PlanExecutorImpl::customiseStep(const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail)
	{
		//TD13442++
		try
        {
            // Move to PlanAgentCorbaDefImpl::customiseStep, just check in Control mode
            // In the monitor mode, Active plan state may be not ready
// 			TA_Base_Core::EActivePlanState planState =  ActivePlanBulletinBoard::instance()->getState( plan );
// 			if ( planState != TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE && planState != TA_Base_Core::PENDING_ACTIVE_PLAN_STATE )
// 			{
// 				TA_THROW( TA_Base_Core::PlanAgentException ("An active plan can only be customised in PAUSED or PENDING state.", TA_Base_Core::PAE_WRONG_STATE_TO_CUSTOMISE_ACTIVE_PLAN) );
// 			}
			ActivePlanCustomiseStepMessage* customiseStepMessage = new ActivePlanCustomiseStepMessage( detail );
			sendMessage( plan, customiseStepMessage );
		}
		catch ( ... )
        {
            // Do nothing as plan state was not found.
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Plan state not found");
        }
		//++TD13442
	}

    void PlanExecutorImpl::updateActivePlanDetail( TA_Base_Core::ActivePlanDetail& detail )
    {
        // Bad logic if ActivePlanDetail update is received when the plan is not start running.
//         if ( !ActivePlanBulletinBoard::instance()->existsPlan( detail.plan ) )
//         {
// 			if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE == detail.activePlanState )
// 			{
//                 run( detail.owner.in(), "", detail.plan );
//                 return;
//             }
//         }

        if ( 0 == detail.currentStepDetail.position )
        {
            // indicate start running plan. do nothing
            LOG_GENERIC( SourceInfo, DEBUG_LOG_LEVEL, "Plan [ %d, %d ] start to run", detail.plan.plan, detail.plan.instance );
            return;
        }
		
		//[[ CL-21338 wzy , dead loop issue
		const unsigned char maxSleepNumberOfTime = 20;
		const unsigned char sleepTimeEachTime = 50;
		unsigned char indexSleep = 0;
        while ( !ActivePlanBulletinBoard::instance()->existsPlan( detail.plan ) )
		{
			TA_Base_Core::Thread::sleep( sleepTimeEachTime );
			indexSleep++;
			if (indexSleep > maxSleepNumberOfTime)
			{
				LOG_GENERIC(SOURCE_INFO, TA_Base_Core::DebugUtil::DebugError, "Not found the Plan [%d, %d] in [%d] millisecond, break the loop", 
					detail.plan.plan, detail.plan.instance, maxSleepNumberOfTime * sleepTimeEachTime);
				break;
			}
		}
		//]]

		//TD13442++
		try
        {
// 			TA_Base_Core::EActivePlanState state = ActivePlanBulletinBoard::instance()->getState( detail.plan );
// 
// 			if (( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == state ) ||
// 				 ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == state ) ||
// 				 ( TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE == state ))
// 			{
// 				return;
// 			}

            if ( ActivePlanBulletinBoard::instance()->isActive( detail.plan ))
            {
                ActivePlanDetailUpdateMessage* planDetailMessage = new ActivePlanDetailUpdateMessage( detail );
			    sendMessage( detail.plan, planDetailMessage );
            }
            else
            {
                if ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == detail.activePlanState )
                {
                    ActivePlanBulletinBoard::instance()->cullStoppedPlan( detail.plan );
                }
                else
                {
                    // Update the step state to complete or un-executed but not loaded
                    ActivePlanBulletinBoard::instance()->updateActivePlanDetail( detail );
                }
            }
		}
		catch(TA_Base_Core::TransactiveException& ex) //CL-21338 wzy
		{
			LOG_EXCEPTION_CATCH(SOURCE_INFO, "TransactiveException", ex.what());
		}
		catch ( ... )
        {
            // Do nothing as plan state was not found.
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Plan state not found");
        }
		//++TD13442
    }

	void PlanExecutorImpl::sendMessage( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* controlMessage )
	{
		std::auto_ptr<ActivePlanMessageBlock> message( new ActivePlanMessageBlock( plan, controlMessage ) );

		m_executionEngine->put( message.release() );
	}
    
    void PlanExecutorImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
        ActivePlanCustomiseStepMessage* customiseStepMessage = new ActivePlanCustomiseStepMessage( historyUpdate );
        sendMessage( historyUpdate.plan, customiseStepMessage );
    }

    void PlanExecutorImpl::clearAllPlans()
    {
        m_executionEngine->clearAllMessageQueue();
    }

    void PlanExecutorImpl::signalAllPlansIntoControl()
    {
        m_executionEngine->signalAllPlansIntoControl();
    }
} // TA_Base_App
