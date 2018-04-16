/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable:4503)
#endif // _MSC_VER


#include "ace/OS.h"

#include "app/signs/tis_agent/src/TISAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;

int main( int argc, char* argv[] )
{
    FUNCTION_ENTRY("TISAgent");
    try
    {
		TA_IRS_App::TISAgent agent(argc, argv);
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
