/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/LogStopCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/LogStopCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;


LogStopCommand::LogStopCommand(CommandOwner * owner)
: LeafCommand(owner, "stop")
{
}

bool LogStopCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if ((args.size() < 1) || (args.size() > 2))
    {
        err << "ERROR: insufficient parameters. Expected 1 or 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    query->setType(LogStop);

    std::string tableName = args.front();
    args.pop_front();
    //
    // first check: is it a table name ?
    TableIdentifier tableId = ATSSimUtility::string2TableIdentifier(tableName);
    if (tableId!=INVALID_IDENTIFIER)
    {
        location = mergeStations(tableId, location);

        //
        // from which we figure the table address
        unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
        if (tAddr == TableAddressRegister::INVALID_ADDRESS)
        {
            err << "ERROR: could not determine table address for table " << tableName << ", id=" << tableId << std::endl;
            return false;
        }

        query->setTableAddress(tAddr);
    }

    //
    // What other arguments do we have here ?
    if (!args.empty())
    {
        std::string argsie = args.front();
        args.pop_front();

        if ( (query->setRecords(argsie, 250 /* MAX_RECORDS */)) > 0 )
        {
            // no-op.  The query records have already been set
        }
        else
        {
            err << "ERROR: unrecognised 'log stop' argument: " << argsie << std::endl;
            return false;
        }
    }
    
    return true;
}


void LogStopCommand::usage(const std::string & name, std::ostream & os) const
{
    os << getName() << " <table name> [record range]" << std::endl;
}

