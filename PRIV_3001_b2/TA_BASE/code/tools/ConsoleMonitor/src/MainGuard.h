#ifndef INCLUDE_MAIN_GUARD_H
#define INCLUDE_MAIN_GUARD_H

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/RunParams.h"


class MainGuard
{
public:

    MainGuard(int argc, char* argv[])
    {
        TA_Base_Core::RunParams::getInstance().parseCmdLine( argc, argv );
        TA_Base_Core::gSetDebugUtilFromRunParams();
        TA_Base_Core::CorbaUtil::getInstance().initialise();
        TA_Base_Core::CorbaUtil::getInstance().activate();
    }

    ~MainGuard()
    {
        TA_Base_Core::CorbaUtil::getInstance().shutdown();
        TA_Base_Core::CorbaUtil::cleanup();
    }
};


#endif
