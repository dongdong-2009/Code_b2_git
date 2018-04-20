/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/StatusCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Definition/Implementation file for CommandExample classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/StatusCommand.h"
#include "bus/generic_server_simulator/src/GssSimulationModule.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_Base_Bus;


StatusCommand::StatusCommand(CommandOwner * owner, GssSimulationModule * simulation_module)
: LeafCommand(owner, "status")
, m_simulationModule(simulation_module)
{
}

bool StatusCommand::execute(TokenList & args, const std::string & argstr)
{
    if ( m_simulationModule == 0 )
    {
        std::cout << "No module current" << std::endl;
        return true;
    }

    //
    // basic argument checking
    if (args.size() > 1)
    {
        std::cerr << "WARNING: too many parameters. Expected 0, got "
                  << args.size() << ": " << argstr << std::endl;
    }
    
    return m_simulationModule->execStatus();
}


void StatusCommand::usage(const std::string & name) const
{
    std::cout << name << std::endl;
}

