/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetRegisterCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/SetRegisterCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace TA_IRS_App::PA_Sim;

SetRegisterCommand::SetRegisterCommand(CommandOwner * owner)
: LeafCommand(owner, "register")
{
}

bool SetRegisterCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    // basic argument checking
    if (args.size() < 3)
    {
        err << "ERROR: insufficient parameters. Expected at least 3, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    query->m_startAddress = atoi(args.front().c_str());
    args.pop_front();
    
    query->m_endAddress = atoi(args.front().c_str());
    args.pop_front();

    int registerNum = query->m_endAddress - query->m_startAddress + 1;
	int registerValue = -1;
	while (registerNum--)
	{
		try
		{
			if ( !args.empty())
			{
				registerValue = TA_IRS_App::PA_Sim::PASimUtility::string2int( args.front() );
				query->m_data.push_back( MSB(registerValue) );
				query->m_data.push_back( LSB(registerValue) );
				args.pop_front();
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError , "SetRegisterCommand fail");
			break;
		}

	}
    return true;
}


void SetRegisterCommand::usage(const std::string & name, std::ostream & os) const
{
    os << "register <start address> <end address> <value>... " << std::endl;
}