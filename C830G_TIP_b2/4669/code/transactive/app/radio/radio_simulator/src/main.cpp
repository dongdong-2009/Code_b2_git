#pragma warning (disable : 4786)
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
//System Includes
#include <iostream>

//TransActive Includes
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"

#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioCommandMonitor.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::GssUtility;

int main(int argc, char* argv[])
{
    try
    {
        if( TA_Base_Bus::GssUtility::initialiseUtilities(argc, argv) )
        {
            // only set the --debug-level if it wasn't on the command line
            if ( ! RunParams::getInstance().isSet(RPARAM_DEBUGLEVEL) )
            {
                DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
            }

            // only set the --debug-file if it wasn't on the command line
            if ( ! RunParams::getInstance().isSet(RPARAM_DEBUGFILE) )
            {
                DebugUtil::getInstance().setFile("./RadioSimulator.log");
            }

            // only set the --debug-file-max-size if it wasn't on the command line
            if ( ! RunParams::getInstance().isSet(RPARAM_DEBUGFILEMAXSIZE) )
            {
                // 10 MegaBytes per log file
                DebugUtil::getInstance().setMaxSize(10000000);
            }

            srand(0);		//seed the random number generator

            TA_Base_Bus::GenericSimulator * simulator = new TA_Base_Bus::GenericSimulator();

            TA_IRS_Bus::RadioCommandMonitor* monitor = new TA_IRS_Bus::RadioCommandMonitor("Radio Simulator", simulator);
            monitor->monitorUserInput();

            //once the command monitor returns (user has typed exit), kill the simulator 
            std::cout << "Shutting down simulator..." << std::endl;
			//TD 9830, Bug 477, by James, Delete one line here

            std::cout << "done." << std::endl;
        }
    }
    catch (TA_Base_Core::TransactiveException te)
    {
        std::cerr << "Transactive Exception caught: " << te.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Non-TA Exception caught" << std::endl;
        throw;
    }

	return 0;
}
