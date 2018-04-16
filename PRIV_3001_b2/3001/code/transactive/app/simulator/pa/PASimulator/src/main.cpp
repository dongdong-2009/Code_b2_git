/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulator/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

//System Includes
#include <iostream>

//TransActive Includes
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "ace/ACE.h"

//Threads to be started
#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "bus/generic_server_simulator/src/GssCommandMonitor.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulatorModuleCreator.h"

using TA_Base_Core::DebugUtil;

int main(int argc, char* argv[])
{

    try
    {
        if( TA_Base_Bus::GssUtility::initialiseUtilities(argc, argv) )
        {
            srand(0);		//seed the random number generator

            //
            // Module Creator instantiation
            //
            new TA_IRS_App::PA_Sim::PASimulatorModuleCreator("PA");

            TA_Base_Bus::GenericSimulator * simulator = new TA_Base_Bus::GenericSimulator();

            TA_Base_Bus::GssCommandMonitor * monitor = new TA_Base_Bus::GssCommandMonitor("Generic Server Simulator", simulator);
            monitor->monitorUserInput();

            //once the command monitor returns (user has typed exit), kill the simulator 
            std::cout << "Shutting down simulator..." << std::endl;
            delete simulator;

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

    return false;
}