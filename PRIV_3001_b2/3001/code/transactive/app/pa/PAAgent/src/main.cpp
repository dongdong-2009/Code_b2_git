/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "app/pa/PAAgent/src/StationPAAgent.h"
#include "app/pa/PAAgent/src/MasterPAAgent.h"


int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("PAAgent");

    try
    {
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
