/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/IntermediateCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
//#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"

#include "app/ats/ATSSimulatorLibrary/cmd/IntermediateCommand.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;



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

