/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/ShowModeCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowModeCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulatorCommandProcessor.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;


ShowModeCommand::ShowModeCommand(CommandOwner * owner)
: LeafCommand(owner, "mode")
{
}

bool ShowModeCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() > 1)
    {
        err << "WARNING: too many parameters. Expected 0 or 1, got "
            << args.size() << ": " << argstr << std::endl;
    }
    
    query->setType(Mode);

    //
    // Need a table address for the query to be used.  Any address will do,
    // but the watchdog is everywhere so we use that.
    unsigned short tAddr = TableAddressRegister::getInstance().getAddress(IscsWatchdog, location);
    if (tAddr == TableAddressRegister::INVALID_ADDRESS)
    {
        err << "ERROR: could not determine table address for IscsWatchdog, id=" << (int)IscsWatchdog << " (how embarrassing)" << std::endl;
        return false;
    }
    query->setTableAddress(tAddr);

    if ( args.size() == 1 )
    {
        std::string formPAtr = args.front();
        args.pop_front();

        //
        // The PASimTableQueryProcessor will be handling this query. The
        // protocol here is that if no arguments are present, it will print
        // the current show mode. If any arguments are given, the output format
        // specified in the query will be used to set the mode.  This is
        // actually a little crunchy, since there really ought to be a
        // 'set mode' command.
        if (formPAtr=="hex")
        {
            query->setOutputFormat(f_hex);
            query->pushArg(0, 0);
        }
        else if (formPAtr=="tabular")
        {
            query->setOutputFormat(f_tabular);
            query->pushArg(0, 0);
        }
        else
        {
            err << "ERROR: unrecognised format specifier: " << formPAtr << std::endl;
        }
    }

    return true;
}


void ShowModeCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " [hex|tabular]" << std::endl;
}

