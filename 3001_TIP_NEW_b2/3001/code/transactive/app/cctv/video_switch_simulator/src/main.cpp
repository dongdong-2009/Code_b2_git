/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_simulator/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

//System Includes
#include <iostream>

//TransActive Includes
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COECommandMonitor.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::UtilityInitialiser;

int main(int argc, char* argv[])
{

	if( true == (TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv)) )
	{	
        // taken verbatim from UtilityInitialiser::initialiseDebugUtil()
        // Set the debug file name
        std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE);
        if (!param.empty())
        {
            DebugUtil::getInstance().setFile(param.c_str());
        }

        // Set the debug level
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
        if (!param.empty())
        {
            DebugUtil::getInstance().setLevel(DebugUtil::getDebugLevelFromString(param.c_str()));
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
	}

	srand(0);		//seed the random number generator

	TA_Base_Bus::GenericSimulator* simulator = new TA_Base_Bus::GenericSimulator();

	TA_IRS_App::COECommandMonitor* monitor = new TA_IRS_App::COECommandMonitor("COE Video Switch Simulator", simulator);
	monitor->monitorUserInput();

	//once the command monitor returns (user has typed exit), kill the simulator 
	std::cout << "Shutting down simulator..." << std::endl;
	delete simulator;
	std::cout << "done." << std::endl;

	return 0;
}