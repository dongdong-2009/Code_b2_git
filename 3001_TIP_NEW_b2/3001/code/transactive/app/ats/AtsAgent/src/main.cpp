/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/02/21 17:43:06 $
  * Last modified by:  $Author: jiayi.zhang $
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4259)
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "app/ats/AtsAgent/src/AtsAgent.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_IRS_App::AtsAgent;

int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("AtsAgent");
	
	TA_Base_Core::RunParams::getInstance().parseCmdLine( argc, argv );
	if (RunParams::getInstance().isSet(RPARAM_VERSION))
	{
		// Versioning info printed to stdout. Not an exceptional condition,
		// but there's no need to continue.
		return 0;
	}
	
    try
    {
		AtsAgent* agent = new AtsAgent(argc, argv);
		agent->run();
		delete agent;
    }
    catch ( const GenericAgentException& gae )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }
    catch ( const TA_Base_Core::DatabaseException& dbe )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "DatabaseException", dbe.what());
    }
    catch ( const TA_Base_Core::DataException& de )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "DataException", de.what());
    }
	catch (...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "Unknown Exception", "Unknown exception caught in Main");
	}

    FUNCTION_EXIT;
	return 0;
}
