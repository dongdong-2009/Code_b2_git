/**
*  The source code in this file is the property of
*  Ripple Systems and is not for redistribution
*  in any form.
*
*  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAgentCorbaDefImpl.cpp $
*  @author:  Gregg Kirkpatrick
*  @version: $Revision: #25 $
*
*  Last modification: $DateTime: 2015/11/02 18:24:29 $
*  Last modified by:  $Author: qi.huang $
*
*  Implementation of DAI::PlanAgentDataCorbaDef and GenericAgent
*  interfaces.
*/

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/PlanDataWriter.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanAgentCorbaDefImpl.h"
#include "app/response_plans/common/src/StringConstants.h"


namespace TA_Base_App
{
	
	PlanAgentCorbaDefImpl::PlanAgentCorbaDefImpl(PlanAgentUser * parentClass) :
    m_parentClass(parentClass)
	{
	}

	PlanAgentCorbaDefImpl::~PlanAgentCorbaDefImpl()
	{
    }
	
	CORBA::Boolean PlanAgentCorbaDefImpl::isOwner(const char* session, const TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("isOwner");

		checkOperationMode();

        CORBA::Boolean retVal = false;

		try
		{
            retVal = ActivePlanBulletinBoard::instance()->isOwner(session, plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError( static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch( ...)
		{
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown exception"));
        }

        FUNCTION_EXIT;
        return retVal;
	}

	void PlanAgentCorbaDefImpl::runPlan(const char* session, TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("runPlan");

		runPlan( session, plan, 0u, false, true );

        FUNCTION_EXIT;
	}
    
    void PlanAgentCorbaDefImpl::runPlan( const char* session, TA_Base_Core::ActivePlanId& plan, const unsigned long ulTriggerEntity, const bool bUniqueTrigger, bool checkActionPermitted )
	{
		FUNCTION_ENTRY("runPlan");

		checkOperationMode();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "runPlan: session=%s, planId=%lu, triggerEntity=%lu, checkActionPermited=%s",
            session, plan.plan, ulTriggerEntity, checkActionPermitted ? "true" : "false" );

		if( checkActionPermitted )
        {
            std::string reason = "";
			if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CONTROL_PLAN, reason))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
				FUNCTION_EXIT;
				throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
			}
		}

        if ( bUniqueTrigger )
        {
            std::list<TA_Base_Core::ActivePlanId> lstActiveInstances;
            ActivePlanBulletinBoard::instance( )->getActiveInstances( plan.plan, lstActiveInstances, ulTriggerEntity, true );

            if ( !lstActiveInstances.empty() )
            {
                std::ostringstream planException;
                planException << "Entity " << ulTriggerEntity << " is restricted to running only a single instance of a plan. " << "Currently running plan id " << plan.plan;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, planException.str().c_str() );
                TA_THROW( TA_Base_Core::PlanAgentException( planException.str(), TA_Base_Core::PAE_EXCEEDED_RUNNING_PLANS_FOR_ENTITY ) );
            }
        }

		try
		{
			PlanExecutor::instance()->run( session, plan, ulTriggerEntity );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown exception"));
		}
        FUNCTION_EXIT;
	}

    unsigned long PlanAgentCorbaDefImpl::runPlanByNameWithoutActionPermittedCheck(const char* session, const char* path, const char* param)
	{
		FUNCTION_ENTRY( "runPlanByNameForTSP" );

		checkOperationMode();

        TA_Base_Core::ActivePlanId plan;
        plan.plan = getNodeId(path);
        
        runPlan(session, plan, 0u, false, false);

		FUNCTION_EXIT;
        return plan.instance;
	}
	
	unsigned long PlanAgentCorbaDefImpl::runPlanByName(const char* session, const char* path, const char* param)
	{
		FUNCTION_ENTRY("runPlanByName");

		checkOperationMode();

        TA_Base_Core::ActivePlanId plan;
        plan.plan = getNodeId(path);

        runPlan(session, plan);
		
        FUNCTION_EXIT;
        return plan.instance;
	}
	

	//Prevents another instance of a plan to execute while a plan of the same type is executing for the same entity
	unsigned long PlanAgentCorbaDefImpl::runPlanByNameWithEntityRestriction( const char* session, const char* path, unsigned long entitykey)
	{
		FUNCTION_ENTRY("runPlanByNameWithEntityRestriction");

		checkOperationMode();

        TA_Base_Core::ActivePlanId plan;
        plan.plan = getNodeId( path );

        runPlan(session, plan, entitykey, true, true );
		
        FUNCTION_EXIT;
        return plan.instance;
	}
	

    void PlanAgentCorbaDefImpl::runOperatorTriggeredPlan(const char* session, TA_Base_Core::ActivePlanId& plan)
    {
        FUNCTION_ENTRY("runOperatorTriggeredPlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CONTROL_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanExecutor::instance()->run( session, plan, false, true );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot run operator triggered plan"));
		}
		
        FUNCTION_EXIT;
    }
		
	void PlanAgentCorbaDefImpl::customisePlan(const char* session, TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("customisePlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CUSTOMISE_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanExecutor::instance()->run( session, plan, true, true );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot customise plan"));
		}
		
        FUNCTION_EXIT;
	}

	void PlanAgentCorbaDefImpl::pausePlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("pausePlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CONTROL_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

        const unsigned long ulInstanceNum = plan.instance;

		try
		{
			PlanExecutor::instance()->pause( plan );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_DETAILS( SourceInfo, typeid(e).name(), e.what() );
			if (ActivePlanBulletinBoard::instance()->isManualPlan (plan))
            {
                const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).detail.path.in();
				AAMessageSender::instance()->submitAuditPlanPauseFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_PLAN_PAUSE_ERROR, 0, e.what()));
			}
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
			if (ActivePlanBulletinBoard::instance()->isManualPlan (plan))
            {
                const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).detail.path.in();
                AAMessageSender::instance()->submitAuditPlanPauseFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_PLAN_PAUSE_ERROR, 0, "Cannot pause plan"));
			}
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::resumePlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("resumePlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CONTROL_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).detail.path.in();
        const unsigned long ulInstanceNum = plan.instance;

		try
		{
			PlanExecutor::instance()->resume( plan );

			AAMessageSender::instance()->submitAuditPlanResume( session, strPlanName, ulInstanceNum );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
			if (ActivePlanBulletinBoard::instance()->isManualPlan(plan))
            {
                AAMessageSender::instance()->submitAuditPlanResumeFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_PLAN_RESUME_ERROR, 0, e.what()));
			}
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
			if (ActivePlanBulletinBoard::instance()->isManualPlan(plan))
            {
                AAMessageSender::instance()->submitAuditPlanResumeFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_PLAN_RESUME_ERROR, 0, "Cannot resume plan"));
			}
		}

        FUNCTION_EXIT;
	}
    
	void PlanAgentCorbaDefImpl::stopPlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("stopPlan");

		checkOperationMode();

        std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CONTROL_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

        reason = ActivePlanBulletinBoard::instance()->getOwner(session);
        reason += RemarkOperatorStopPlan;

        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).detail.path.in();
        const unsigned long ulInstanceNum = plan.instance;
		
		try
		{
			PlanExecutor::instance()->stop( plan, reason );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
			if (ActivePlanBulletinBoard::instance()->isManualPlan (plan))
			{
				AAMessageSender::instance()->submitAuditPlanStopFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_PLAN_STOP_ERROR, 0, e.what()));
			}
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
			if (ActivePlanBulletinBoard::instance()->isManualPlan (plan))
            {
                AAMessageSender::instance()->submitAuditPlanStopFailure( session, strPlanName, ulInstanceNum );
				FUNCTION_EXIT;
				throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_PLAN_STOP_ERROR, 0, "Cannot stop plan"));
			}
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::takeControl(const char* session, const TA_Base_Core::ActivePlanId& plan)
	{
		FUNCTION_ENTRY("takeControl");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CHANGE_PLAN_OWNER, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		// TODO: talk to the current controller first, then do this:
		try
		{
			PlanExecutor::instance()->reassign( session, plan );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot take control"));
		}

        FUNCTION_EXIT;
	}
	
	
	void PlanAgentCorbaDefImpl::customiseStep(const char* session, const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail)
	{
		FUNCTION_ENTRY("customiseStep");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_CUSTOMISE_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
        {
			int planState =  ActivePlanBulletinBoard::instance()->getPlanState( plan );

			if ( planState != TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE )
			{
				TA_THROW( TA_Base_Core::PlanAgentException ("An active plan can only be customised in PAUSED state.", TA_Base_Core::PAE_WRONG_STATE_TO_CUSTOMISE_ACTIVE_PLAN) );
			}

			PlanExecutor::instance()->customiseStep( plan, detail );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot customise step"));
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::ExecutionHistory_out history)
	{
		FUNCTION_ENTRY("getPlanExecutionHistory");

		checkOperationMode();

		try
		{
			history = ActivePlanBulletinBoard::instance()->getPlanExecutionHistory( plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unhandled exception in getPlanExecutionHistory()."));
		}

        FUNCTION_EXIT;
	}
    
	
	void PlanAgentCorbaDefImpl::getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::CustomisationHistory_out history)
	{
		FUNCTION_ENTRY("getPlanCustomisationHistory");

		checkOperationMode();

		try
		{
			history = ActivePlanBulletinBoard::instance()->getPlanCustomisationHistory( plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unhandled exception in getPlanCustomisationHistory()."));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::setPlanApprovalState(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState state)
	{
		FUNCTION_ENTRY("setPlanApprovalState");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_APPROVE_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

        try
        {
            PlanDataWriter::instance()->setPlanApprovalState(session, plan, state);
        }
        catch ( const TA_Base_Core::PlanStepDetailException& e )
        {
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanStepDetailError( e.getStepPosition(), e.getErrorType(), e.getDetailType().c_str(), e.what() ));
        }
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot set plan approval state"));
		}
		
        FUNCTION_EXIT;
	}

    void PlanAgentCorbaDefImpl::setPlanType(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType state)
    {
        FUNCTION_ENTRY("setPlanVisibleState");

        checkOperationMode();

        try
        {
            PlanDataWriter::instance()->setPlanType(session, plan, state);
        }
        catch( const TA_Base_App::PlanNotFoundException& e)
        {
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
        }
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
        catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
        }
        catch( ...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot set plan approval state"));
        }

        FUNCTION_EXIT;
    }

	void PlanAgentCorbaDefImpl::getRootNodeChildren(TA_Base_Core::SiblingNodes_out children)
	{
		FUNCTION_ENTRY("getRootNodeChildren");

		checkOperationMode();

		try
		{
			children = TA_Base_Bus::PlanDataReader::instance()->getChildren( TA_Base_Core::ROOT_NODE_ID);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
		{
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getChildren(TA_Base_Core::NodeId parent, TA_Base_Core::SiblingNodes_out children)
	{
		FUNCTION_ENTRY("getChildren");

		checkOperationMode();
		
		try
		{
			children = TA_Base_Bus::PlanDataReader::instance()->getChildren( parent);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
		{
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
        FUNCTION_EXIT;
	}

	void PlanAgentCorbaDefImpl::getNodeDetail(TA_Base_Core::NodeId node, TA_Base_Core::NodeDetail_out detail)
	{
		FUNCTION_ENTRY("getNodeDetail");

		checkOperationMode();
		
		try
		{
			detail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( node );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
		{
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getNodePath(TA_Base_Core::NodeId node, TA_Base_Core::NodePath_out path)
	{
		FUNCTION_ENTRY("getNodePath");

		checkOperationMode();

		try
		{
			path = CORBA::string_dup( TA_Base_Bus::PlanDataReader::instance()->getNodePath( node).c_str());
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
		{
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
        FUNCTION_EXIT;
	}
	
    void PlanAgentCorbaDefImpl::getNodeIdsInNodePath(TA_Base_Core::NodeId plan, TA_Base_Core::NodeIds_out plans)
	{
		FUNCTION_ENTRY("getNodeIdsInNodePath");

		checkOperationMode();
		
		try
		{
			plans = TA_Base_Bus::PlanDataReader::instance()->getNodeIdsInNodePath( plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
        FUNCTION_EXIT;
	}
	
    TA_Base_Core::NodeId PlanAgentCorbaDefImpl::getNodeId(const char* path)
    {
		FUNCTION_ENTRY("getNodeId");

		checkOperationMode();

        TA_Base_Core::NodeId nodeId = TA_Base_Bus::PlanDataReader::instance()->getNodeId(path);

        FUNCTION_EXIT;
        return nodeId;
    }
	
	void PlanAgentCorbaDefImpl::getNodeSummary(TA_Base_Core::NodeId node, TA_Base_Core::NodeSummary_out summary)
	{
		FUNCTION_ENTRY("getNodeSummary");

		checkOperationMode();
		
		try
		{
			summary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(node);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
        FUNCTION_EXIT;
	}
	
    void PlanAgentCorbaDefImpl::getStepPosition(TA_Base_Core::NodeId planId, TA_Base_Core::StepId stepId, TA_Base_Core::StepNumber& position)
	{
		FUNCTION_ENTRY("getStepNumber");
		
		checkOperationMode();

		try
		{
            position = TA_Base_Bus::PlanDataReader::instance()->getStepPosition(planId, stepId);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
        FUNCTION_EXIT;
	}
	
	
	void PlanAgentCorbaDefImpl::getActivePlanDetail(TA_Base_Core::NodeId plan, TA_Base_Core::ActivePlanDetails_out details)
	{
		FUNCTION_ENTRY("getActivePlanDetail");

		checkOperationMode();

		try
		{
			details = ActivePlanBulletinBoard::instance()->getActivePlanDetails(plan) ;
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot get active plan detail"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getAssocAlarmPlanDetail(const char* alarmId, TA_Base_Core::ActivePlanDetail_out detail)
	{
		FUNCTION_ENTRY("getAssocAlarmPlanDetail");

		checkOperationMode();

		try
        {
			detail = new TA_Base_Core::ActivePlanDetail( ActivePlanBulletinBoard::instance()->getAssocAlarmActivePlanDetail( alarmId ));
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot get active plan detail"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getActivePlanDetailByName(const char* path, TA_Base_Core::ActivePlanDetails_out details)
	{
		FUNCTION_ENTRY("getActivePlanDetailByName");
		
		checkOperationMode();
		
        getActivePlanDetail(getNodeId(path), details);
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getAllActivePlanDetails(TA_Base_Core::ActivePlanStepsDetails_out details)
	{
		FUNCTION_ENTRY("getAllActivePlanDetails");

		checkOperationMode();

		try
		{
			details = ActivePlanBulletinBoard::instance()->getAllActivePlanDetails();
		}
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot get all active plan details"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getEditPlanLock(const char* session, TA_Base_Core::NodeId plan)
	{
		FUNCTION_ENTRY("getEditPlanLock");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->getEditPlanLock(session, plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot get plan lock"));
		}
		
        FUNCTION_EXIT;
	}
	
	
	void PlanAgentCorbaDefImpl::releaseEditPlanLock(const char* session, TA_Base_Core::NodeId plan)
	{
		FUNCTION_ENTRY("releaseEditPlanLock");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->releaseEditPlanLock(session, plan);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch( const std::exception& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Cannot release lock"));
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getStepNames(TA_Base_Core::StepNames_out names)
	{
		FUNCTION_ENTRY("getStepNames");

		checkOperationMode();

		try
		{
			names = TA_Base_Bus::PlanDataReader::instance()->getStepNames();
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::getAllStepDetails(TA_Base_Core::NodeId plan, TA_Base_Core::StepDetails_out details)
	{
		FUNCTION_ENTRY("getAllStepDetails");

		checkOperationMode();

		try
		{
			details = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( plan);
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
        FUNCTION_EXIT;
    }

    void PlanAgentCorbaDefImpl::getSequenceFlows( TA_Base_Core::NodeId plan, TA_Base_Core::SequenceFlows_out flows )
    {
        FUNCTION_ENTRY( "getSequenceFlows" );

        checkOperationMode();

        try
        {
            flows = TA_Base_Bus::PlanDataReader::instance()->getSequenceFlows( plan );
        }
        catch ( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what() ));
        }
        catch ( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what() ));
        }
        catch ( const std::exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what() ));
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error" ));
        }

        FUNCTION_EXIT;
    }

	void PlanAgentCorbaDefImpl::getSpecifiedStepDetails(TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps, TA_Base_Core::StepDetails_out details)
	{
		FUNCTION_ENTRY("getSpecifiedStepDetails");
		
		checkOperationMode();
		
		try
		{
			details = TA_Base_Bus::PlanDataReader::instance()->getSpecifiedStepDetails( plan, steps);
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}

        FUNCTION_EXIT;
	}
	
    void PlanAgentCorbaDefImpl::getParameterNames( CORBA::Long stepType, TA_Base_Core::ParameterNames_out names )
	{
		FUNCTION_ENTRY("getParameterNames");
		
		checkOperationMode();
		
		try
		{
			names = TA_Base_Bus::PlanDataReader::instance()->getParameterNames( stepType);
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}

        FUNCTION_EXIT;
	}
	
    void PlanAgentCorbaDefImpl::getParametersByStepId( TA_Base_Core::StepId step, TA_Base_Core::StepParameters_out parameters )
	{
		FUNCTION_ENTRY("getStepIdParameters");

		checkOperationMode();
		
		try
		{
			parameters = TA_Base_Bus::PlanDataReader::instance()->getStepParameters( 0, step );
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}

        FUNCTION_EXIT;
	}
	
    void PlanAgentCorbaDefImpl::getParametersByStepNumber( TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber step, TA_Base_Core::StepParameters_out parameters )
	{
		FUNCTION_ENTRY("getStepNumberParameters");
		
		checkOperationMode();
		
		try
		{
			parameters = TA_Base_Bus::PlanDataReader::instance()->getStepParameters( plan, step, true );
		}
        catch ( const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what());
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}

        FUNCTION_EXIT;
	}
	
	
	void PlanAgentCorbaDefImpl::updateCategoryDetail(const char* session, const TA_Base_Core::CategoryDetail& detail)
	{
		FUNCTION_ENTRY("updateCategoryDetail");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_CATEGORY, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanDataWriter::instance()->updateCategory(session, detail);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in updateCategory()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::insertCategoryIntoRoot(const char* session)
	{
		FUNCTION_ENTRY("insertCategoryIntoRoot");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_CATEGORY, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanDataWriter::instance()->insertCategory(session, TA_Base_Core::ROOT_NODE_ID);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in insertCategoryIntoRoot()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::insertCategory(const char* session, TA_Base_Core::NodeId parent)
	{
		FUNCTION_ENTRY("insertCategory");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_CATEGORY, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->insertCategory(session, parent);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in insertCategory()"));
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::deleteCategory(const char* session, TA_Base_Core::NodeId category)
	{
		FUNCTION_ENTRY("deleteCategory");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_CATEGORY, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanDataWriter::instance()->deleteCategory( session, category);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in deleteCategory()"));
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::updatePlanDetail(const char* session, const TA_Base_Core::PlanDetail& detail)
	{
		FUNCTION_ENTRY("updatePlanDetail");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->updatePlanDetail( session, detail );
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in updatePlanDetail()"));
		}
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::updatePlan(const char* session, const TA_Base_Core::Plan& plan)
	{
		FUNCTION_ENTRY("updatePlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->updatePlan( session, plan );
		}
        catch (const TA_Base_Core::PlanStepDetailException& e)
        {
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanStepDetailError(e.getStepPosition(), e.getErrorType(), e.getDetailType().c_str(), e.what()));
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in updatePlan()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::insertPlanIntoRoot(const char* session, TA_Base_Core::ENode nodeType)
	{
		FUNCTION_ENTRY("insertPlanIntoRoot");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->insertPlan( session, TA_Base_Core::ROOT_NODE_ID, nodeType);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in insertPlanIntoRoot()"));
		}

        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::insertPlan(const char* session, TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
	{
		FUNCTION_ENTRY("insertPlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->insertPlan(session, parent, nodeType);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in insertPlan()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::deletePlans(const char* session, const TA_Base_Core::NodeIds& plans)
	{
		FUNCTION_ENTRY("deletePlans");
		
		checkOperationMode();
		
		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->deletePlans( session, plans);
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in deletePlans()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::movePlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce)
	{
		FUNCTION_ENTRY("movePlanToRoot");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->movePlan( session, plan, TA_Base_Core::ROOT_NODE_ID, useForce);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanNotFoundException", e.what());
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "PlanAgentException", e.what());
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in movePlanToRoot()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::movePlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce)
	{
		FUNCTION_ENTRY("movePlan");

		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->movePlan( session, fromPlan, toParent, useForce);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in movePlan()"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::copyPlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce)
	{
		FUNCTION_ENTRY("copyPlanToRoot");
		
		checkOperationMode();
		
		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->copyPlan( session, plan, TA_Base_Core::ROOT_NODE_ID, useForce);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in copyPlanToRoot()"));
		}
		
        FUNCTION_EXIT;
		
	}
	
	void PlanAgentCorbaDefImpl::copyPlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce)
	{
		FUNCTION_ENTRY("copyPlan");

		checkOperationMode();
		
		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}
		
		try
		{
			PlanDataWriter::instance()->copyPlan( session, fromPlan, toParent, useForce);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
            FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what());
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown Exception in copyPlan()"));
		}
		
        FUNCTION_EXIT;
	}
	
	
	void PlanAgentCorbaDefImpl::getNextStepId(const char* session, TA_Base_Core::StepId& step)
	{
		FUNCTION_ENTRY("getNextStepId");

		checkOperationMode();
		
		try
		{
			step = TA_Base_Bus::PlanDataReader::instance()->getNextPlanSeqNumber();
		}
        catch (const TA_Base_Core::PlanAgentException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(e.getErrorType(), e.getItemId(), e.what()));
        }
        catch ( const TA_Base_Core::DataException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
        }
        catch ( const TA_Base_Core::DatabaseException& e)
        {
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
        }
		catch( const std::exception& e)
		{
            FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
        FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::exportPlan (const char* session, TA_Base_Core::AuxPlanDetail_out plan, TA_Base_Core::NodeId planId)
	{
		FUNCTION_ENTRY("exportPlan");
		
		checkOperationMode();

		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
			FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			plan = PlanDataWriter::instance()->exportPlan (session, planId);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
			FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch ( const TA_Base_Core::DataException& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch ( const TA_Base_Core::DatabaseException& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
		}
		catch( const std::exception& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}
		
		FUNCTION_EXIT;
	}
	
	void PlanAgentCorbaDefImpl::importPlan (const char* session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
	{
		FUNCTION_ENTRY("importPlan");
		
		checkOperationMode();
		
		std::string reason = "";
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, TA_Base_Bus::aca_PLAN_EDIT_PLAN, reason))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", reason.c_str());
            FUNCTION_EXIT;
			throw (TA_Base_Bus::IPlanAgentCorbaDef::NoPermission(reason.c_str()));
		}

		try
		{
			PlanDataWriter::instance()->importPlan (session, plan, parentId);
		}
		catch( const TA_Base_App::PlanNotFoundException& e)
		{
			FUNCTION_EXIT;
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
		}
		catch ( const TA_Base_Core::DataException& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATA_ERROR, 0, e.what()));
		}
		catch ( const TA_Base_Core::DatabaseException& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_DATABASE_ERROR, 0, e.what()));
		}
		catch( const std::exception& e)
		{
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
		}
		catch( ...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
			FUNCTION_EXIT;
			throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown error"));
		}

		FUNCTION_EXIT;
	}

	void PlanAgentCorbaDefImpl::checkOperationMode()
	{
		TA_ASSERT(m_parentClass != NULL, "The parent class is NULL");
		m_parentClass->checkOperationMode();
	}
    
    void PlanAgentCorbaDefImpl::processStepResponse( const char* session, const ::TA_Base_Core::ActivePlanId& plan, const ::TA_Base_Core::InteractiveStepData& stepDetail )
    {
        FUNCTION_ENTRY("processStepResponse");

        checkOperationMode();

        try
        {
            if (( ActivePlanBulletinBoard::instance()->isOwner( session, plan )) && 
                ( ActivePlanBulletinBoard::instance()->isActive( plan )) && 
                ( TA_Base_Core::LOADED_ACTIVE_STEP_STATE == ActivePlanBulletinBoard::instance()->getStepState( plan, stepDetail.id )))
            {
                std::vector<std::string> vecParameters;
                
                for(size_t index = 0u; index < stepDetail.parameters.length(); ++index)
                {
                    vecParameters.push_back( stepDetail.parameters[index].in() );
                }

                PlanExecutor::instance()->processStepResponse( stepDetail.type, plan, stepDetail.id, vecParameters );
            }
        }
        catch( const TA_Base_App::PlanNotFoundException& e)
        {
            FUNCTION_EXIT;
            throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(static_cast<TA_Base_Core::EPlanNotFoundError>(e.getErrorType()), e.getPlanId(), e.getInstanceId(), e.what()));
        }
        catch( const std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError(TA_Base_Core::PAE_UNKNOWN_ERROR, 0, e.what()));
        }
        catch( ...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled.");
            FUNCTION_EXIT;
            throw( TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError( TA_Base_Core::PAE_UNKNOWN_ERROR, 0, "Unknown exception"));
        }

        FUNCTION_EXIT;
    }
    
} // TA_Base_App



