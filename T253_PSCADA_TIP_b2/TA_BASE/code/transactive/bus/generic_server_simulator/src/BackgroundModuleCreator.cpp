/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_server_simulator/src/BackgroundModuleCreator.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation file for the BackgroundModuleCreator class
  */

#include "bus/generic_server_simulator/src/BackgroundModuleCreator.h"
#include "bus/generic_server_simulator/src/BackgroundSimulationModule.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include <list>
#include <string>

#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

ISimulationModule * BackgroundModuleCreator::createSimulationModule(const std::string & name, const std::string & parameters)
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

    return new BackgroundSimulationModule(name, host, ports);
}



BackgroundModuleCreator::~BackgroundModuleCreator()
{

}


BackgroundModuleCreator::BackgroundModuleCreator(const std::string & type)
: GssAbstractModuleCreator(type)
{
}

