/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/ShowRegisterCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for PASimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowRegisterCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;


ShowRegisterCommand::ShowRegisterCommand(CommandOwner * owner)
: LeafCommand(owner, "register")
{
}

bool ShowRegisterCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    
    // basic argument checking
    if ( args.size() != 2)
    {
        err << "ERROR: insufficient parameters. Expected 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }
    
    query->m_startAddress = atoi(args.front().c_str());
    //query->m_startAddress = PASimUtility::hex2int(args.front());
    args.pop_front();

    //query->m_endAddress = PASimUtility::hex2int(args.front());
    query->m_endAddress = atoi(args.front().c_str());
    
    return true;
}


void ShowRegisterCommand::usage(const std::string & name, std::ostream & os) const
{
    os << "register <start address> <end address>" << std::endl;
}

