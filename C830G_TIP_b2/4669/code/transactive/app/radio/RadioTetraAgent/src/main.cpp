/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "RadioTetraAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"

using TA_IRS_App::RadioTetraAgent;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;

#include "ace/OS.h"

int main( int argc, char* argv[] )
{
//	FUNCTION_ENTRY("RadioTetraAgent");
    try
    {
        RadioTetraAgent* agent = new RadioTetraAgent(argc, argv);
		agent->run();
		delete agent;
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }

//    FUNCTION_EXIT;
	return 0;
}
