#pragma once
#include "core/utilities/src/DebugUtil.h"

struct TimerUtilTestFixture
{
    TimerUtilTestFixture()
    {
        using namespace TA_Base_Core;
        system("DEL /F TimerTest*.log >NUL 2>NUL");
        DebugUtil::getInstance().setMaxSize(30 * 1024 * 1024);
        DebugUtil::getInstance().setFile("TimerTest.log");
        DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
        ACE::init();
    }

    ~TimerUtilTestFixture()
    {
        ACE::fini();
    }
};
