/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/IntermediateCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"

#include "app/simulator/pa/PASimulatorLibrary/cmd/IntermediateCommand.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;



////////////////////////////////////////////////////////////////
//
// IntermediateCommand
//
////////////////////////////////////////////////////////////////
IntermediateCommand::IntermediateCommand(CommandOwner * owner, const char * name)
: HierarchicalCommand(owner, name)
{
}


IntermediateCommand::~IntermediateCommand( )
{
}


bool IntermediateCommand::isCommand(const std::string & name)
{
    return (name == getName());
}


bool IntermediateCommand::hasSubCommand(const std::string & name)
{
    return (find(name) != 0);
}


bool IntermediateCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
	if ( args.empty() )
	{
		err << "ERROR: \"" << getName() << "\" command requires subcommand parameters." << std::endl;
	}

    //
    // snatch the front arg, it should be the name of our subcommand
    std::string subcmdstr = args.front();
    args.pop_front();

    HierarchicalCommand * subcmd = find(subcmdstr);

    //
    if ( subcmd != 0 )
    {
        return (subcmd->makeQuery(args, argstr, query, location, err));
    }
	else
	{
		err << "ERROR: no subcommand \"" << subcmdstr << "\" for command " << getName() << std::endl;
	}

    return false;
}


void IntermediateCommand::usage(const std::string & name, std::ostream & os) const
{
	const CommandMap & cmds = getCommands();

    for ( CommandMap::const_iterator iter = cmds.begin() ;
            iter != cmds.end() ;
            iter++
        )
    {
        os << getName() << " ";
        ((*iter).second)->usage((*iter).first, os);
    }
}

