/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "app/ecs/ECSAgent/src/CtaEcsAgentSummary.h"
#include "app/ecs/ECSAgent/src/StationECSAgent.h"
#include "app/ecs/ECSAgent/src/MasterECSAgent.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"



static const std::string RUN_PARAM_AGENT_MODE =            "AgentMode";
static const std::string RUN_PARAM_AGENT_MODE_MASTER =     "Master";
static const std::string RUN_PARAM_AGENT_MODE_STATION =    "Station";

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/main.cpp $
  * @author:  Sean Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the main entry point for the application.  
  *
  * Wierd stuff -
  *
  * 1. The FUNCTION_ENTRY macro uses the term "typeid(*this).name()" for log details.  In purify, this generates
  *    Invalid Pointer Reads and Writes everytime the macro is called.  It sounds a bit scary and it may be 
  *    causing VC++ to crash fairly often during debugging.  Not sure if this problem occurs in the release 
  *    build.
  *    
  *    Solutions:
  *    a.  Remove the call to the name() method altogether.
  *    b.  Replace name() with a raw_name() call.  This does not cause a fault in purify but it means that the
  *        class name is mangled 
  *
  *    Will investiage further to see if this is a real concern.
  *
  * 2. A heap of memory leaks in the code when run in purify.  Most of them caused by singletons not being 
  *    explicitly removed.  Where possible, one should supply a thread-safe removeInstance() method for each
  *    singleton.  The main() function should call the removeInstance() methods for all singletons just before 
  *    exiting.
  *
  * 3. Lots of problems with mismatched memory allocation/frees.  All went away when the project was explicitly 
  *    set to include msvcrtd.lib and msvcprtd.lib and ignore msvcrt.lib,msvcprt.lib.
  *
  * 4. Possible issues with the Oracle oci.dll as this uses msvcrt.dll whereas everything else uses mscvrtd.dll.
  *    This issue may go way when compiled in release mode.  Tried to set all libraries to be release ones to 
  *    avoid mixing debug with release libraries but got stuck with symbol linking errors when switching from
  *    mfc42d.dll to mfc42.dll.      
  *    
  */

int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("ECSAgent");


    try
    {
        // Need to obtain arguments earlier than normal so we can determine 
        // whether to run as a Master or Station ECS Agent. 
        // TD14797, don't go further when version checking
        if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine(argc,argv) )
        {
            if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_VERSION))
            {
                // Versioning info printed to stdout. Not an exceptional condition,
                // but there's no need to continue.
                return 0;
            }
            TA_THROW (TA_Base_Core::InvalidCommandLineException("Command line arguments are in invalid format"));
        }

        std::string agentMode = TA_Base_Core::RunParams::getInstance().get(RUN_PARAM_AGENT_MODE.c_str());

        // Note - order is important.  Assuming that these TA interfaces will be 
        // destructed in reverse order when the scope ends.  

        CtaEcsAgentSummary theSummary;
        if (RUN_PARAM_AGENT_MODE_MASTER == agentMode)
        {            
            MasterECSAgent agent(
                argc,
                argv, 
                theSummary);
            
            theSummary.setup();
            theSummary.getItaEcsCachedConfigInitialiser().setIsMaster(true);
            agent.setupSpecifics();

		    agent.run();
        }
        else if (RUN_PARAM_AGENT_MODE_STATION == agentMode)
        {
            StationECSAgent agent(
                argc,
                argv, 
                theSummary);
            theSummary.setup();
            theSummary.getItaEcsCachedConfigInitialiser().setIsMaster(false);
            agent.setupSpecifics();
		    agent.run();
        }
        else
        {
            // Unrecognised AgentMode
            std::ostringstream buffer;
            buffer <<  "Invalid parameter AgentMode = " << agentMode;
            TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::AGENT_ERROR, buffer.str().c_str()));
        }
        
    }
    catch( const TA_Base_Core::GenericAgentException& gae )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "GenericAgentException", gae.what());

    }
    catch( std::exception & e )
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, e.what() );       
	}
	catch( ... )
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
	}

    // Additional clean-up code
    TA_IRS_Core::ECSStationModeAccessFactory::removeInstance();
    TA_IRS_Core::ECSMasterModeAccessFactory::removeInstance();
    TA_IRS_Core::ECSZoneAccessFactory::removeInstance();
    TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

    

    FUNCTION_EXIT;
	return 0;
}



/* 

  Things to worry about later:

  1. unsigned long v.s. int implicit conversions.  See sun compiler output to track down.
  2. Implement "ToDo"s
  3. Unrem "PutMeBack"s
  6. All entity configuration parameters need to be onlineupdatable.
  8. Container safe auto_ptr (boost) needs to be used when accessing DataPointNobjects in 
     StationAgentsController::setStationControlModeDP
  9. Maybe 8 not needed.  Verify that Nobject assignment works properly.
  10. Datapoint online updates.
  11. Turn off receiveMessage in ATSSubscriber when in monitor mode?
  12. Message subscription Access Violation when message senders are deleted. 
  15. a. New MasterMode A
      b. Emergency condition is automatically gone.
      c. Since Current Master Mode is 1002 (Normal) the alarm for MasterMode A is still around.
      Should remove the alarms.

  New stuff
  1. Move CongestionApplied and EmergencyApplied functionality from the EcsZoneDatasource
  2. MasterModeLocksManager may need profiling - perhaps only react to transitions from 1-0
  3. Add volatile to CachedConfig and CachedMaps classes - problems with compiler on UNIX.
  4. Better mechanism for atomicising updates to the CompleteAgentState structures.
  5. Concerns if the Control State takes too long to execute, abortIfRequired will hang until
     the control phase is complete.
  6. Address exceptions especially when getting datapoint/proxy values.
  7. Consolidate exceptions
  8. Add OperationMode, MissingConfig exception.


  Consitency checks:

  2. Verify that all station managed ECS Zone DataPoints are used as decision components.




*/

