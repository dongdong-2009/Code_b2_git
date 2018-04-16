/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/LogModeCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/LogModeCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulatorCommandProcessor.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;


LogModeCommand::LogModeCommand(CommandOwner * owner)
: LeafCommand(owner, "mode")
{
}

bool LogModeCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() > 2)
    {
        err << "WARNING: too many parameters. Expected 2 or less, got "
            << args.size() << ": " << argstr << std::endl;
    }
    
    query->setType(LogMode);

    //
    // Need a table address for the query to be used.  The protocol for
    // specifying to PASimTableLogSupport that no table is specified, is to
    // set the table address to NOTINUSE. So that is what we do by default
    query->setTableAddress(TableAddressRegister::NOTINUSE_ADDRESS);

    while ( ! args.empty() )
    {
        std::string argsie = args.front();
        args.pop_front();

        TableIdentifier tableId = PASimUtility::string2TableIdentifier(argsie);
        if (tableId!=INVALID_IDENTIFIER)
        {
            location = mergeStations(tableId, location);

            //
            // from which we figure the table address
            unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
            if (tAddr == TableAddressRegister::INVALID_ADDRESS)
            {
                err << "ERROR: could not determine table address for table " << argsie << ", id=" << tableId << std::endl;
                return false;
            }

            query->setTableAddress(tAddr);
        }
        //
        // The PASimTableQueryProcessor will be handling this query. The
        // protocol here is that if no arguments are present, it will print
        // the current show mode. If any arguments are given, the output format
        // specified in the query will be used to set the mode.  This is
        // actually a little crunchy, since there really ought to be a
        // 'set mode' command.
        else if (argsie=="hex")
        {
            query->setOutputFormat(f_hex);
            query->pushArg(0, 0);
        }
        else if (argsie=="tabular")
        {
            query->setOutputFormat(f_tabular);
            query->pushArg(0, 0);
        }
        else
        {
            err << "ERROR: unrecognised 'log mode' argument: " << argsie << std::endl;
        }
    }

    return true;
}


void LogModeCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " [table name] [hex|tabular]" << std::endl;
}

