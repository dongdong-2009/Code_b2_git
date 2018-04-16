/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveShutdownSystemStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2012/12/28 15:35:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_SHUTDOWN_SYSTEM )

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <sys/resource.h>    // For setpriority
#endif //#ifdef WIN32

#ifdef SOLARIS
#include <sys/types.h>		 //
#include <sys/processor.h>   // All three needed for processor binding
#include <sys/procset.h>     //
#include <errno.h>
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/process_management/IDL/src/IShutdownCorbaDef.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveShutdownSystemStep.h"
#include "app/system_control/system_controller/src/ProcessUtilities.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveShutdownSystemStep::ActiveShutdownSystemStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::ShutdownSystemParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
        m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveShutdownSystemStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Decision step %lu", getStepNumber() );

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        bool bNotifyPeer = m_params.notifyPeer;

        int port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());
        std::string myHostname = getHostname();

        // Create a string in the form: "corbaloc::hostname:port/ShutdownExecutor"
        std::stringstream shutdownExecutorIor;
        shutdownExecutorIor << "corbaloc::" << myHostname << ":" << port << "/ShutdownExecutor";

        // Attempt to resolve an IShutdownCorbaDef object
        CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( shutdownExecutorIor.str() );

        if ( !CORBA::is_nil(obj) )
        {
            try
            {
                TA_Base_Core::IShutdownCorbaDef_var shutdownExecutorVar = TA_Base_Core::IShutdownCorbaDef::_narrow( obj );
                shutdownExecutorVar->shutdown(CORBA::Boolean(bNotifyPeer));
                m_flowControl.setNextStep();
                stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
            }
            catch (CORBA::SystemException& e)
            {
                m_remark = RemarkStepExecutionFailed;
                std::string desc = "System exception occurred while resolving peer shutdown executor ";
                desc += shutdownExecutorIor.str() + " - ";
                desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
            }
            catch ( ... )
            {
                m_remark = RemarkStepExecutionFailed;
                std::string desc = "Error occurred narrowing ";
                desc += shutdownExecutorIor.str();
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot resolve peer Shutdown Executor %s", shutdownExecutorIor.str().c_str());
        }

		return( stepState );
	}

    std::string ActiveShutdownSystemStep::getHostname()
    {
        char name[32];
        int length = 32;
        gethostname(name, length);

        return name;
    }

} // TA_Base_App


#endif //  STEPTYPE_SHUTDOWN_SYSTEM
