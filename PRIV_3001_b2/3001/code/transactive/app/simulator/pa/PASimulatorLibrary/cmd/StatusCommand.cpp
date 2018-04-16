/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/StatusCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Definition/Implementation file for PASimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/StatusCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulator.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;


StatusCommand::StatusCommand(CommandOwner * owner, PASimulator * simulator)
: LeafCommand(owner, "status")
, m_simulator(simulator)
{
}

bool StatusCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() > 1)
    {
        err << "WARNING: too many parameters. Expected 0, got "
            << args.size() << ": " << argstr << std::endl;
    }
    
    return m_simulator->execStatus(err);
}


void StatusCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << std::endl;
}

