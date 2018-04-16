/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutor.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #17 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/threads/src/Thread.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanReassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanUnassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanExecuteStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanSharedDataMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepResponseMessage.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/StepImplementationFactory.h"
#include "app/response_plans/plan_agent/src/PlanInfoCache.h"


namespace TA_Base_App
{
	PlanCacheMap m_mapPlanCache; // Cache map to store Plan & Step info based on Plan ID.

	PlanExecutorImpl::PlanExecutorImpl() :
	m_executionEngine(new PlanExecutionEngine())
	{
        if ( -1 == m_executionEngine->open( 0 ) )
        {
            std::string message = "Active plan thread pool could not be created.";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );
            TA_THROW( TA_Base_Core::PlanAgentException( message.c_str(), TA_Base_Core::PAE_INITIALISATION_FAILURE ) );
        }
	}

	PlanExecutorImpl::~PlanExecutorImpl()
	{
		delete m_executionEngine;
	}

	void PlanExecutorImpl::waitForRunningPlans( void )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing : %s", "waitForRunningPlans enter" );

		if( NULL != m_executionEngine )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing : %s", "if branch enter" );

            m_executionEngine->terminateAndWait();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing : %s", "if branch exit" );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing : %s", "waitForRunningPlans exit" );
	}
    
    void PlanExecutorImpl::run( const std::string& session, TA_Base_Core::ActivePlanId& plan, const unsigned long entity, const bool bPending, const bool manExecution, const bool bStartFromMonitorMode )
	{
        TA_Base_Core::NodeDetail* nodeDetail = NULL;
        std::ostringstream message;

		try
		{			
			boost::shared_ptr<PlanStepInfo> psi = PlanInfoCache::instance()->findPlanInfo(plan.plan);
			if (psi.get() != NULL && psi->nodeDetails.get() != NULL)
			{
				nodeDetail = psi->nodeDetails.get();
			}
			else
			{
				nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail(plan.plan);
				boost::shared_ptr<PlanStepInfo> info(new PlanStepInfo);
				info->nodeDetails.reset(nodeDetail);
				PlanInfoCache::instance()->addPlanInfo(plan.plan, info);
			}
		}
		catch ( ... )
		{
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] not found.";
            TA_THROW(PlanNotFoundException(message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, plan.plan, plan.instance));
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if (( nodeType != TA_Base_Core::PLAN_NODE ) && ( nodeType != TA_Base_Core::WORKFLOW_NODE ) && (nodeType != TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE))
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

		m_executionEngine->run( session, plan, bPending, entity, manExecution, bStartFromMonitorMode );
	}

	void PlanExecutorImpl::spawnPending(const std::string& session, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bStartFromMonitorMode )
	{
		std::ostringstream message;

		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( childPlan.plan );
		}
		catch (...)
		{
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] not found.";
            TA_THROW( PlanNotFoundException( message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, childPlan.plan, childPlan.instance) );
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if (( nodeType != TA_Base_Core::PLAN_NODE ) && ( nodeType != TA_Base_Core::WORKFLOW_NODE ) && (nodeType != TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE))
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

		m_executionEngine->spawn( session, parentPlan, childPlan, true, bStartFromMonitorMode );
	}

    void PlanExecutorImpl::stop(const TA_Base_Core::ActivePlanId& plan, const std::string& reason)
	{
		ActivePlanStopMessage* stopMessage = new ActivePlanStopMessage( reason );
		sendMessage( plan, stopMessage );
	}

	void PlanExecutorImpl::complete(const TA_Base_Core::ActivePlanId& plan)
	{
        ActivePlanCompleteMessage* completeMessage = new ActivePlanCompleteMessage();
		sendMessage( plan, completeMessage );
	}

	void PlanExecutorImpl::pause(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanPauseMessage* pauseMessage = new ActivePlanPauseMessage;
		sendMessage( plan, pauseMessage );
	}

	void PlanExecutorImpl::resume(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanResumeMessage* resumeMessage = new ActivePlanResumeMessage();
		sendMessage( plan, resumeMessage );
	}

	void PlanExecutorImpl::reassign(const std::string& session, const TA_Base_Core::ActivePlanId& plan )
	{
		ActivePlanReassignMessage* reassignMessage = new ActivePlanReassignMessage(session);
		sendMessage( plan, reassignMessage );
	}

	void PlanExecutorImpl::unassign(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanUnassignMessage* unassignMessage = new ActivePlanUnassignMessage;
		sendMessage( plan, unassignMessage );
    }

    void PlanExecutorImpl::executeStep(const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::ActiveStepDetail& detail)
    {
        const unsigned char maxSleepNumberOfTime = 20;
        const unsigned char sleepTimeEachTime = 50;
        unsigned char indexSleep = 0;
        while ( !ActivePlanBulletinBoard::instance()->existsPlan( plan ))
        {
            TA_Base_Core::Thread::sleep( sleepTimeEachTime );
            indexSleep++;
            if (indexSleep > maxSleepNumberOfTime)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found the Plan [%lu, %lu] in [%d] millisecond, break the loop", 
                    plan.plan, plan.instance, maxSleepNumberOfTime * sleepTimeEachTime);
                break;
            }
        }

        try
        {
            if ( ActivePlanBulletinBoard::instance()->isActive( plan ))
            {
                ActivePlanExecuteStepMessage* executeStepMessage = new ActivePlanExecuteStepMessage( detail );
                sendMessage( plan, executeStepMessage );
            }
        }
        catch ( ... )
        {
            // Do nothing as plan state was not found.
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Plan state not found");
        }
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
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Plan state not found");
        }
		//++TD13442
	}

    void PlanExecutorImpl::updateActivePlanDetail( TA_Base_Core::ActivePlanDetail& detail )
    {
        const unsigned char maxSleepNumberOfTime = 20;
		const unsigned char sleepTimeEachTime = 50;
		unsigned char indexSleep = 0;
        while ( !ActivePlanBulletinBoard::instance()->existsPlan( detail.plan ) )
		{
			TA_Base_Core::Thread::sleep( sleepTimeEachTime );
			indexSleep++;
			if (indexSleep > maxSleepNumberOfTime)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found the Plan [%d, %d] in [%d] millisecond, break the loop", 
					detail.plan.plan, detail.plan.instance, maxSleepNumberOfTime * sleepTimeEachTime);
				break;
			}
		}

		//TD13442++
		try
        {
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
		catch(TA_Base_Core::TransactiveException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		}
		catch ( ... )
        {
            // Do nothing as plan state was not found.
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Plan state not found");
        }
		//++TD13442
    }

	void PlanExecutorImpl::sendMessage( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* controlMessage )
	{
        if ( NULL == controlMessage )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Trying to send NULL message but rejected" );
            return;
        }

		std::auto_ptr<ActivePlanMessageBlock> message( new ActivePlanMessageBlock( plan, controlMessage ) );

		m_executionEngine->put( message.release() );
	}

    void PlanExecutorImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
        ActivePlanCustomiseStepMessage* customiseStepMessage = new ActivePlanCustomiseStepMessage( historyUpdate );
        sendMessage( historyUpdate.plan, customiseStepMessage );
    }

    void PlanExecutorImpl::processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate )
    {
        ActivePlanExecuteStepMessage* executeStepMessage = new ActivePlanExecuteStepMessage( historyUpdate );
        sendMessage( historyUpdate.plan, executeStepMessage );
    }

    void PlanExecutorImpl::processPlanSharedDataUpdate( TA_Base_Core::ActivePlanSharedData& detail )
    {
        ActivePlanSharedDataMessage* sharedDataMessage = new ActivePlanSharedDataMessage( detail.sharedData );
        sendMessage( detail.plan, sharedDataMessage );
    }

    void PlanExecutorImpl::processStepResponse( const int eType, const TA_Base_Core::ActivePlanId& plan, const unsigned int unStepId, const std::vector<std::string>& parameters )
    {
        IActiveStepMessage* pStepMsg = ActiveStepFactory::instance()->createStepMessage( eType, unStepId, parameters );

        ActivePlanStepResponseMessage* planMessage = new ActivePlanStepResponseMessage( pStepMsg );
        sendMessage( plan, planMessage );
    }

    void PlanExecutorImpl::changedToMonitor()
    {
        m_executionEngine->signalAgentModeChanged( false );
    }

    void PlanExecutorImpl::changedToControl()
    {
        m_executionEngine->signalAgentModeChanged( true );
    }

} // TA_Base_App
