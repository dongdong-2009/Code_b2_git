/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "app/pa/PAAgent/src/StationPAAgent.h"
#include "app/pa/PAAgent/src/MasterPAAgent.h"

#include "ace/Object_Manager.h"


int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("PAAgent");

    try
    {
        ACE_Object_Manager::instance()->init();

        // Need to obtain arguments earlier than normal so we can determine 
        // whether to run as a Master or Station PA Agent.  
        TA_Base_Core::RunParams::getInstance().parseCmdLine(argc,argv);

		std::auto_ptr<TA_IRS_App::AbstractPAAgent> paAgent(NULL);

        std::string agentMode = TA_Base_Core::RunParams::getInstance().get("AgentMode");

        if ("Master" == agentMode)
        {
            paAgent.reset( new TA_IRS_App::MasterPAAgent(argc,argv) );
            paAgent->run();
        }
        else if ("Station" == agentMode)
        {
            paAgent.reset( new TA_IRS_App::StationPAAgent(argc,argv) );
            paAgent->run();
        }
        else if ( !TA_Base_Core::RunParams::getInstance().isSet(RPARAM_VERSION) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The agent mode [%s] is not recognizable", agentMode.c_str() );
        }

        paAgent.reset();

        ACE_Object_Manager::instance()->fini();
    }
    catch( std::exception& expWhat )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );       
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception" );
	}

    FUNCTION_EXIT;
	return 0;
}
