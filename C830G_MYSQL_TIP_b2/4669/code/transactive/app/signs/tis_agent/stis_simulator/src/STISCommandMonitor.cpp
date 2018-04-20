/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/stis_simulator/src/STISCommandMonitor.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Implementation file for the CommandMonitor class.  
  * CommandMonitor provides functionality to constantly listen for
  * user input from the command line, and to process that input
  * and execute the appropriate commands.
  */

#include <iostream>


#include "app/signs/tis_agent/stis_simulator/src/STISSimulator.h"
#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "app/signs/tis_agent/stis_simulator/src/STISCommandMonitor.h"


// TODO: these should not be here
//#include "PeekSimulator.h"
//#include "DataGenerator.h"

namespace TA_IRS_App
{
	/* ----------------------------------------------------------------------------
	 * Construction/Destruction
	 * --------------------------------------------------------------------------*/
	STISCommandMonitor::STISCommandMonitor(const std::string &desc, TA_Base_Bus::GenericSimulator *simulator)
	 : CommandMonitor(desc,simulator)
	{

	}

	STISCommandMonitor::~STISCommandMonitor()
	{
	}

	TA_Base_Bus::ISimulationModule* STISCommandMonitor::createSimulationModule(const std::string& type,
																   const std::string& name,
																   const std::string& parameters)
	{
		if (type=="STIS")
		{
			char    host[100];
			char    port[100];
			char	location[10];

			// Get the host and port 
			int numTokens = sscanf(parameters.c_str(), "%s %s", port, location);
			if (numTokens==2)
			{
				std::string locationStr(location);
				return new TA_IRS_App::STISSimulator(name,port,locationStr);
			}
			else
			{
				std::cout << "Incorrect parameters for STIS simulator module." << name << std::endl;
				std::cout << "addmod <name> STIS <port> <location>" << std::endl;

			}
		}

		return NULL;
	}

}
