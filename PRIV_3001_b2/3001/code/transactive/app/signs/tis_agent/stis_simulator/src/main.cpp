/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/stis_simulator/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#pragma warning (disable : 4786)

#include "ace/OS.h"

//System Includes
#include <iostream>

//TransActive Includes
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/RunParams.h"

//Threads to be started
#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "app/signs/tis_agent/stis_simulator/src/STISCommandMonitor.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::UtilityInitialiser;

namespace TA_Base_Core
{
    // External versioning variables
    // See VersionGen.cpp
    //
    extern char buildVersion[];
    extern char buildDate[];
    extern char componentName[];
    extern char componentVersion[];
    extern int headerArraySize;
    extern char* headerArray[];
}
int main(int argc, char* argv[])
{

	if( true == (TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv)) )
	{
        std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE);
        if (!param.empty())
        {
            DebugUtil::getInstance().setFile(param.c_str());
        }
        else
        {
            DebugUtil::getInstance().setFile("./stisSimulator.log");
        }

        // Set the debug level
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
        if (!param.empty())
        {
            DebugUtil::getInstance().setLevel(DebugUtil::getDebugLevelFromString(param.c_str()));
        }
        else
        {
            DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
        }

        // Set the maximum size of a debug file in bytes
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
        if (!param.empty())
        {
            DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
        }

        // Set the maximum number of log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
        if (!param.empty())
        {
            DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
        }
        else
        {
            TA_Base_Core::RunParams::getInstance().set(RPARAM_DEBUGFILEMAXSIZE, "9999999999999999");
        }

		//Check the command line for host/port info.  If there is no command, set defaults.
		if( false == TA_Base_Core::RunParams::getInstance().isSet("Host") )
		{
			TA_Base_Core::RunParams::getInstance().set("Host", "localhost");
		}

		if( false == TA_Base_Core::RunParams::getInstance().isSet("Port") )
		{
			TA_Base_Core::RunParams::getInstance().set("Port", "50");
		}
	}


	srand(0);		//seed the random number generator

	TA_Base_Bus::GenericSimulator* simulator = new TA_Base_Bus::GenericSimulator();

    TA_IRS_App::STISCommandMonitor* monitor = new TA_IRS_App::STISCommandMonitor(TA_Base_Core::componentName, simulator);
	monitor->monitorUserInput();

	//once the command monitor returns (user has typed exit), kill the simulator 
	std::cout << "Shutting down simulator..." << std::endl;
	delete simulator;
	std::cout << "done." << std::endl;

	return 0;
}
