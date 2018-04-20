
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/examples/echo_server/src/EchoSimulationModule.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  * Definition of the EchoSimulationModule class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/examples/echo_server/src/EchoSimulationModule.h"
#include "bus/generic_server_simulator/examples/echo_server/src/EchoCommsObserver.h"



using namespace TA_Base_Bus;


EchoSimulationModule::EchoSimulationModule
(   const std::string & name,
    const std::string & host,
    const PortList & ports
)
: GssSimulationModule( name, host, ports )
{
}


GssCommsObserver * EchoSimulationModule::createCommsObserver( GssCommsObserver::ObservedSocketType * socket )
{
    return new EchoCommsObserver( socket );
}


std::string EchoSimulationModule::getSimDescription()
{
    return std::string( "Echo Server" );
}

