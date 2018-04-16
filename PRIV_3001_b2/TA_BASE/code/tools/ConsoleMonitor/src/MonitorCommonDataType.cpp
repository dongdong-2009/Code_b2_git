#include "MonitorCommonDataType.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src//StdPubUtility.h"


namespace TA_Base_Core
{

    char * TA_Base_Core::InnerMonitorConsts::SET_NORMAL_PARAM = "setParam";
    char * TA_Base_Core::InnerMonitorConsts::SET_CORBA_PARAM = "setCorbaParam";
    char * TA_Base_Core::InnerMonitorConsts::MONITOR_ENTITYNAME = "ConsoleMonitor";

    char * TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_BASE = "base";
    char * TA_Base_Core::InnerMonitorConsts::MODUME_TYPE_COMMON = "common";
    char * TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_NSA = "nsa";


    void MonitorStaticLib::outputDebugString(const char * strContent)
    {
        // Always write the string to std::cout
        std::cout << strContent << std::endl;

        // Only write the string to the log file if a file has been set (otherwise it will
        // be written to std::cout twice).
        if (TA_Base_Core::RunParams::getInstance().isSet("DebugFile"))
        {
            TA_Base_Core::DebugUtil::getInstance().logLargeString(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, strContent);
        }
    }

    void MonitorStaticLib::splitCommand(const char * commandLine, std::vector<std::string> & vtArgs)
    {
        vtArgs.clear();

        char tmpCommandLineBuffer[1024];
        tmpCommandLineBuffer[0] = 0x0;
        strcpy(tmpCommandLineBuffer, commandLine);
        char * szContent = TA_Base_Core::CStdPubUtility::trim(tmpCommandLineBuffer);
        if ( TA_Base_Core::CStdPubUtility::isEmptyString(szContent) )
        {
            return;
        }


        while( !TA_Base_Core::CStdPubUtility::isEmptyString(szContent) )
        {
            char * szLeft = TA_Base_Core::CStdPubUtility::cutToToken(&szContent);
            assert(szLeft != 0x0 && *szLeft != 0x0);
            vtArgs.push_back(szLeft);
        }

    }


}