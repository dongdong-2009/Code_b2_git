/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#if defined (WIN32)
#include <crtdbg.h>
#endif

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/StationPAAgent.h"
#include "app/pa/PAAgent/src/MasterPAAgent.h"

//Maochun Sun++
//#TD11155
#include "core/process_management/src/UtilityInitialiser.h"
//++Maochun Sun
//#TD11155

#include "ace/OS.h"

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/main.cpp $
  * @author:  Sean Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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


using namespace TA_Base_Core;

using namespace TA_IRS_App;

int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("PAAgent");

#if defined (WIN32)
    // Remove leak checking
    _CrtSetDbgFlag(0);
#endif

    try
    {
        // Need to obtain arguments earlier than normal so we can determine 
        // whether to run as a Master or Station PA Agent.  
        RunParams::getInstance().parseCmdLine(argc,argv);

		//Maochun Sun++
		// #TD11155
        if(!UtilityInitialiser::initialiseUtilities(argc, argv))
        {
            return 0;
        }
		//++Maochun Sun
        // #TD11155

		std::auto_ptr<TA_IRS_App::AbstractPAAgent> paAgent(NULL);

        std::string agentMode = RunParams::getInstance().get("AgentMode");

        if ("Master" == agentMode)
        {
            paAgent = std::auto_ptr<TA_IRS_App::AbstractPAAgent>( new MasterPAAgent(argc,argv) );
		    paAgent->run();
        }
        else if ("Station" == agentMode)
        {
            paAgent = std::auto_ptr<TA_IRS_App::AbstractPAAgent>( new StationPAAgent(argc,argv) );
		    paAgent->run();
        }
        else
        {
            // Unrecognised AgentMode
            std::ostringstream buffer;
            buffer <<  "Invalid parameter AgentMode = " << agentMode;
            TA_THROW(TA_Base_Core::GenericAgentException(GenericAgentException::AGENT_ERROR, buffer.str().c_str()));
        }

    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "GenericAgentException", gae.what());

    }
    catch( std::exception & e )
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, e.what() );       
	}
	catch( ... )
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Caught unknown exception" );
	}

#if defined (WIN32)
    // Remove leak checking
    _CrtSetDbgFlag(0);
#endif

    FUNCTION_EXIT;
	return 0;
}



/* 

  Things to worry about later:

  . unsigned long v.s. int implicit conversions.  See sun compiler output to track down.
  . Implement "ToDo"s
  . Unrem "PutMeBack"s
  . All entity configuration parameters need to be onlineupdatable.
  . Container safe auto_ptr (boost) needs to be used when accessing DataPointNobjects in 
     StationAgentsController::setStationControlModeDP
  . Need to revise agent synchronisation model...
  . a. New MasterMode A
    b. Emergency condition is automatically gone.
    c. Since Current Master Mode is 1002 (Normal) the alarm for MasterMode A is still around.
       Should remove the alarms.
  . Need to request updates of all ATS states on startup.
  . Remove "DEMO" blocks
  . Remove "LOGOUT" debug info
  . Table startup and destruction states?
  . TCP/IP socket enabling/disabling?

 

  Consitency checks:

  2. Verify that all station managed PA Zone DataPoints are used as decision components.




*/

