//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/3002/transactive/app/wild/wild_agent/src/main.cpp $
// @author:  Andy Siow
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Main.cpp 
// This is the main cpp file that starts the WILD Agent
//


#include "ace/OS.h"

#include "app/wild/wild_agent/src/WildAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/WILDAgentExceptions.h"

#include <vector>
#include <string>

using TA_IRS_App::WILDAgent;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;

int main( int argc, char* argv[] )
{
    try
    {
		WILDAgent agent(argc, argv);
        agent.run();
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }
    catch( const DatabaseException& dbe )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "DatabaseException", dbe.what());
    }
    catch( const DataException& de )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "DataException", de.what());
    }
	catch(...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
             "Unknown exception caught from the main method!");
	}

    return 0;
}
