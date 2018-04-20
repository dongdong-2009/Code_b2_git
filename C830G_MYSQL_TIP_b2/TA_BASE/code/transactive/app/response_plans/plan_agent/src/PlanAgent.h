#ifndef PLANAGENT_H_INCLUDED
#define PLANAGENT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAgent.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #7 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  General include for the Plan Agent.
  */

// Pragmas
#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( disable : 4786 4018 4146 )
#endif

// System includes
#include <iostream>
#include <memory>
#include <sstream>
#include <limits>
#include <iomanip>

#if defined( WIN32 ) || defined( __WIN32__ )
#include <crtdbg.h>
#endif


#define PLAN_EDIT_LOCK_SEPARATOR ":"

//#include "app/response_plans/common/src/ProjectDefinition.h"

// COTS includes

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"
//#include "ace/Method_Object.h"
#include "ace/Method_Request.h"
#include "ace/OS.h"
#include "ace/Singleton.h"
#include "ace/Process_Semaphore.h"

#include "boost/shared_ptr.hpp"

// Core includes
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/naming/src/Naming.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDai.h"
#include "core/naming/src/NamedObject.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/message/types/PlanAgentAlarms_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/synchronisation/src/Condition.h"

// Business includes
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

// Application includes
//   (CORBA)
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
//   (Exceptions)
#include "app/response_plans/plan_agent/src/ActiveStepParamException.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageException.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"

#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"


//   needed by Active(Jump/Abort)Step
namespace TA_Base_App
{
	class IActivePlanFlow;
};

// Type definitions
//   Must be after the Plan Agent CORBA includes and before
//   ActivePlanBulletinBoard and ActiveTerminatePlanStep
namespace TA_Base_App
{
    typedef std::list<TA_Base_Core::ActivePlanId> ActivePlanList;
    typedef ActivePlanList::iterator ActivePlanListIterator;
    typedef ActivePlanList::const_iterator ConstActivePlanListIterator;
}


// Enum for ActivePlanBulletinBoard and ActivePlan but should also be used
// for returns from all functions that wait for a result
namespace TA_Base_App
{
	enum EWaitResult
	{
		TIMEOUT_WR  ,
		SUCCESS_WR  ,
		FAILURE_WR  ,
		UNDEFINED_WR
	};
}

#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActiveStepsHeader.h"
#include "app/response_plans/plan_agent/src/ActiveEndStep.h"


// Type definitions
//   Must be after ActiveStep and before anything that uses it.
//   PlanAccessFactory and ActivePlan at time of writing.
namespace TA_Base_App
{
	typedef std::list<ActiveStep*> ActiveStepList;
	typedef ActiveStepList::iterator ActiveStepListIterator;
}

#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "bus/response_plans/PlanDataAccess/src/PlansCommon.h"
#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"
#include "app/response_plans/plan_agent/src/PlanDataWriter.h"
#include "app/response_plans/plan_agent/src/PlanAgentCorbaDefImpl.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/AuditMessageSender.h"

#include "app/response_plans/plan_agent/src/SortablePlanId.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"

#include "app/response_plans/plan_agent/src/ActivePlanState.h"
#include "app/response_plans/plan_agent/src/ActivePlanExecutingState.h"
#include "app/response_plans/plan_agent/src/ActivePlanPendingState.h"
#include "app/response_plans/plan_agent/src/ActivePlanPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanOperatorPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanDelayedState.h"

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageQueue.h"
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseForOperatorMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanReassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanUnassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"
#include "app/response_plans/plan_agent/src/ActivePlanDecisionStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLogStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchGuiAppStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.h" //DP-Changes

#include "app/response_plans/plan_agent/src/RunPlanMethodObject.h"
#include "app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"

#include "app/response_plans/plan_agent/src/DataPointUpdateStateSubscriber.h"

#include "app/response_plans/plan_agent/src/GRCType3SubTask.h"

#include "app/response_plans/plan_agent/src/GRC3DataPointSetComplexStep.h"

#include "app/response_plans/plan_agent/src/ActiveNullStep.h"



// Defines
#if defined( WIN32 ) || defined( __WIN32__ )
#define _CRTDBG_MAP_ALLOC
#endif

// Logging defines
#define SOURCE_INFO      SourceInfo

#define GENERIC_LOG_TYPE          TA_Base_Core::DebugUtil::GenericLog
#define EXCEPTION_CATCH_LOG_TYPE  TA_Base_Core::DebugUtil::ExceptionCatch
#define EXCEPTION_DETAIL_LOG_TYPE TA_Base_Core::DebugUtil::ExceptionDetails
#define FUNCTION_ENTRY_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionEntry
#define FUNCTION_EXIT_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionExit

#define FATAL_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugFatal
#define ERROR_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugError
#define WARN_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugWarn
#define INFO_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugInfo
#define DEBUG_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugDebug

const int GRCTYPE3_NORMAL_STEP = 1;
const int GRCTYPE3_PARENT_STEP = 2;
const int GRCTYPE3_CHILD_STEP = 3;


// Macros
// External functions
// External variables
// Constants

// namespace TA_TA_App
// {
// 	namespace TA_TA_ResponsePlans
//  	{
// 		// Forward declarations
// 	}
// }


#endif //PLANAGENT_H_INCLUDED
