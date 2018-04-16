/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable:4503)
#endif // _MSC_VER


#include "ace/OS.h"

#include "app/signs/pids_agent/src/PIDSAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;

int main( int argc, char* argv[] )
{
    FUNCTION_ENTRY("PIDSAgent");
    try
    {
		TA_IRS_App::PIDSAgent agent(argc, argv);
		agent.run();
		//delete agent;
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }

    FUNCTION_EXIT;
	return 0;
}
