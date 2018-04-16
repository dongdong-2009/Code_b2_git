/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_server_simulator/examples/command_example/src/CommandExampleModuleCreator.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Implementation file for the CommandExampleModuleCreator class
  */

#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleModuleCreator.h"
#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleSimulationModule.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include <list>
#include <string>

#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

ISimulationModule * CommandExampleModuleCreator::createSimulationModule(const std::string & name, const std::string & parameters)
{
    std::string host;
	GssSimulationModule::PortList ports;

    std::list<std::string> params;
    GssUtility::tokenise(parameters.c_str(), params);

    if ( params.size() == 2 )
    {
        host = params.front();
        params.pop_front();

        std::string port_str = params.front();
        params.pop_front();

        std::list<std::string> port_params;
        GssUtility::tokenise(port_str.c_str(), ports, ",");
    }
    else
    {
        std::cerr << "ERROR: wrong number of parameters for module. Require <host> <portlist>" << std::endl;
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "ERROR: too few parameters for module. Require <host> <portlist>" );
        return 0;
    }

    return new CommandExampleSimulationModule(name, host, ports);
}



CommandExampleModuleCreator::~CommandExampleModuleCreator()
{

}


CommandExampleModuleCreator::CommandExampleModuleCreator(const std::string & type)
: GssAbstractModuleCreator(type)
{
}

