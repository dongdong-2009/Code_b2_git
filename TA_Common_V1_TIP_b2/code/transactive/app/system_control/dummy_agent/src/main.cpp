#include "StdAfx.h"
#include "DummyAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/versioning/src/Version.h"

using namespace TA_Base_Core;

int main( int argc, char* argv[] )
{
    if (argc == 2 && boost::iequals(argv[1], "--version"))
    {
        Version::displayVersionInfo();
        return 0;
    }

    try
    {		
		DummyAgent agent(argc, argv);
		agent.run();
    }
	catch (...)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "exception of unknown type caught");
	}

	return 0;
}
