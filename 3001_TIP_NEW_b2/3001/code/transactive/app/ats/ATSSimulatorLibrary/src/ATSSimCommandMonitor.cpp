/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimCommandMonitor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * \class ATSSimCommandMonitor
  * Implementation file for the ATSSimCommandMonitor class, derived from the
  * CommandMonitor class. CommandMonitor provides functionality to constantly
  * listen for user input from the command line, and to process that input
  * and execute the appropriate commands.
  *
  * ATSSimCommandMonitor extends CommandMonitor by overriding createSimulationModule
  * to return an instance of the ATSSimulator class.
  */

#include <iostream>


#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommandMonitor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimulator.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "bus/generic_simulator/src/GenericSimulator.h"


// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/
ATSSimCommandMonitor::ATSSimCommandMonitor(const std::string &desc, TA_Base_Bus::GenericSimulator *simulator)
 : CommandMonitor(desc,simulator)
{

}

ATSSimCommandMonitor::~ATSSimCommandMonitor()
{
}

static void usage(std::ostream & os)
{
    os << "Incorrect parameters for simulator module type \"ATS\"" << std::endl << std::endl;
    os << "Usage: addmod <name> ATS <location> <host> <portlist>" << std::endl << std::endl;
    os << "  where:" << std::endl;
    os << "  name        an arbitrary name given to the module instance, usually related" << std::endl;
    os << "              to the location eg. occ or occ_ats" << std::endl;
    os << "  location    one of OCC, STATION or DEPOT" << std::endl;
    os << "  host        the name or IP address of the host to bind to and listen on" << std::endl;
    os << "  portlist    either one port number or a comma-seperated list of ports on" << std::endl;
    os << "              which to listen. See the Simulator Description document for more" << std::endl;
    os << "              details." << std::endl << std::endl;
}

TA_Base_Bus::ISimulationModule * ATSSimCommandMonitor::createSimulationModule
(   const std::string& type,
    const std::string& name,
    const std::string& parameters)
{
    if (type=="ATS")
    {
        char    host[100];
        char    port[100];
        char    location[100];

        LocationType loc = NO_LOCATION;

        //
        // Get the host and port 
		// @todo the host and port should be optional, as they default
		//       to localhost and 25500 respectively
        int numTokens = sscanf(parameters.c_str(), "%s %s %s", location, host, port);

        // validate and convert the location type input string
        if (numTokens>=1)
        {
            // darn VC++ can't implicitly convert char[100] to std::string but
            // CAN find a suitable conversion constructor for std::string
            std::string locationS = location;
            loc = ATSSimUtility::string2LocationType(locationS);
            if ((loc != OCC) && (loc != STATION) && (loc != DEPOT))
            {
                std::cerr << "Incorrect location type, must be one of OCC|STATION|DEPOT" << std::endl;
                ::usage(std::cout);
                return 0;
            }
        }

        //
        // TODO: validate the host address input string
        if (numTokens>=2)
        {
        }

        //
        // Validate the port input string
        // Minimum required is a port number. Other valid input strings are 
        // 1.   number
        // 2.   number,number,number
        // 3.   mss_identifer=number,ss1_identifier=number,ss2_identifier=number
        //
        // for example 
        // 1.   25501
        // 2.   25501,25502,25503
        // 3.   DBG=25501,MSM=25502,CVC=25503
        //
        // All in all, more than is worth validating here, since the string is
        // parsed again in ATSSimulator::ATSSimulator
        //
        if (numTokens>=3)
        {
//             std::list<std::string> portlist=
//             int portnum=atoi(port);
//             if ( portnum < 1 )
//             {
//                 std::cerr << "Invalid port number specified" << std::endl;
//                 return 0;
//             }
        }


        if (numTokens==3)
        {
            return new ATSSimulator(name,loc,host,port);
        }
        else
        {
            ::usage(std::cout);
        }
    }

    return 0;
}
