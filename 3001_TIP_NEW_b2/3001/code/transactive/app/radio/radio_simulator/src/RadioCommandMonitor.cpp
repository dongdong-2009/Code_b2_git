/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioCommandMonitor.cpp $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  * class RadioCommandMonitor
  * Implementation file for the RadioCommandMonitor class, derived from the
  * CommandMonitor class. CommandMonitor provides functionality to constantly
  * listen for user input from the command line, and to process that input
  * and execute the appropriate commands.
  *
  * RadioCommandMonitor extends CommandMonitor by overriding createSimulationModule
  * to return an instance of the OSSSSimulator class.
  */

#include <iostream>


#include "app/radio/radio_simulator/src/RadioCommandMonitor.h"
#include "app/radio/radio_simulator/src/RadioSimulator.h"
#include "bus/generic_simulator/src/GenericSimulator.h"


using namespace TA_IRS_App;

/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/
RadioCommandMonitor::RadioCommandMonitor(const std::string &desc, TA_Base_Bus::GenericSimulator *simulator)
 : CommandMonitor(desc,simulator)
{

}

RadioCommandMonitor::~RadioCommandMonitor()
{
}

TA_Base_Bus::ISimulationModule* RadioCommandMonitor::createSimulationModule
(   const std::string& type,
    const std::string& name,
    const std::string& parameters)
{
    if (type=="RADIO")
    {
        char    host[100];
        char    port[100];

        // Get the host and port 
		// @todo the host and port should be optional, as they default
		//       to localhost and 27000 respectively
        int numTokens = sscanf(parameters.c_str(), "%s %s", host, port);
        if (numTokens==2)
        {
            return new TA_IRS_App::RadioSimulator(name,host,port);
        }
        else
        {
            std::cout << "Incorrect parameters for RADIO simulator module." << name << std::endl;
            std::cout << "addmod <name> RADIO <host> <port>" << std::endl;

        }
    }

    return 0;
}
