
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "app/cctv/tvss_agent/src/TVSSAgent.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_IRS_App::TVSSAgent;

int main( int argc, char* argv[] )
{
	FUNCTION_ENTRY("TVSSAgent");
    try
    {
		TVSSAgent* agent = new TVSSAgent(argc, argv);
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
