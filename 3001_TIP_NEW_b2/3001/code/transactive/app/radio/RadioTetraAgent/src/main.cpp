/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioServerAgentFramework.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;
using namespace TA_IRS_App;

int main( int argc, char* argv[] )
{
	
	// Parse the command line arguments
    TA_Base_Core::RunParams::getInstance().parseCmdLine( argc, argv );
	if (RunParams::getInstance().isSet(RPARAM_VERSION))
	{
		// Versioning info printed to stdout. Not an exceptional condition,
		// but there's no need to continue.
		return 0;
	}
	
    try
    {
        RadioServerAgentFramework agent(argc, argv);

		agent.run();
    }
    catch( const GenericAgentException& gae )
    {
        LOG_GENERIC( SourceInfo, DebugUtil::ExceptionCatch,"GenericAgentException", gae.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown exception caught when running agent");
    }

	return 0;
}

