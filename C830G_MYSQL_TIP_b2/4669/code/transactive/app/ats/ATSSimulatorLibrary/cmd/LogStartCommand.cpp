/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/cmd/LogStartCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/LogStartCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;


LogStartCommand::LogStartCommand(CommandOwner * owner)
: LeafCommand(owner, "start")
{
}

bool LogStartCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if ((args.size() < 1) || (args.size() > 3))
    {
        err << "ERROR: insufficient parameters. Expected 1 to 3, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }


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
    while (!args.empty())
    {
        std::string argsie = args.front();
        args.pop_front();

        //
        // per'aps this one iz le output format specifierre
        if (argsie=="hex")
        {
            query->setOutputFormat(f_hex);
        }
        else if (argsie=="tabular")
        {
            query->setOutputFormat(f_tabular);
        }
        //
        // OR the [optional] records !
        else if ( (query->setRecords(argsie, 250 /* MAX_RECORDS */)) > 0 )
        {
            // no-op.  The query records have already been set
        }
        else
        {
            err << "ERROR: unrecognised 'log' argument: " << argsie << std::endl;
            return false;
        }
    }
    
    return true;
}


void LogStartCommand::usage(const std::string & name, std::ostream & os) const
{
    os << getName() << " <table name> [record range] [hex|tabular]" << std::endl;
}

