/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimulatorModuleCreator.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */

#include "app/simulator/pa/PASimulatorLibrary/src/PASimulatorModuleCreator.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulator.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include <list>
#include <string>

#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App::PA_Sim;
using TA_Base_Bus::GssUtility;
using TA_Base_Bus::ISimulationModule;

void PASimulatorModuleCreator::usage(std::ostream & os)
{
    os << "Usage: addmod <name> PA <location> <host> <portlist>" << std::endl << std::endl;
    os << "  where:" << std::endl;
    os << "  name        an arbitrary name given to the module instance, usually related" << std::endl;
    os << "              to the location eg. occ or occ_PA" << std::endl;
    os << "  location    one of OCC, STATION or DEPOT" << std::endl;
    os << "  host        the name or IP address of the host to bind to and listen on" << std::endl;
    os << "  portlist    either one port number or a comma-seperated list of ports on" << std::endl;
    os << "              which to listen. See the Simulator Description document for more" << std::endl;
    os << "              details." << std::endl << std::endl;
}


ISimulationModule * PASimulatorModuleCreator::createSimulationModule(const std::string & name, const std::string & parameters)
{
    std::string host;
    std::string ports;
    LocationType location = NO_LOCATION;

    std::list<std::string> params;
    GssUtility::tokenise(parameters.c_str(), params);

    if ( params.size() == 3 )
    {
        std::string locationS = params.front();
        params.pop_front();

        host = params.front();
        params.pop_front();

        ports = params.front();
        params.pop_front();

        location = PASimUtility::string2LocationType(locationS);
        if ((location != OCC) && (location != STATION) && (location != DEPOT))
        {
            std::cerr << "Incorrect location type, must be one of OCC|STATION|DEPOT, got:" << locationS << std::endl;
            usage(std::cout);
            return 0;
        }
    }
    else
    {
        std::cerr << "Incorrect parameters for simulator module type \"PA\"" << std::endl << std::endl;
        usage(std::cout);
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "ERROR: too few parameters for module. Require <host> <portlist>" );
        return 0;
    }

    return new PASimulator(name, location, host.c_str(), ports.c_str());
}



PASimulatorModuleCreator::~PASimulatorModuleCreator()
{

}


PASimulatorModuleCreator::PASimulatorModuleCreator(const std::string & type)
: TA_Base_Bus::GssAbstractModuleCreator(type)
{
}

