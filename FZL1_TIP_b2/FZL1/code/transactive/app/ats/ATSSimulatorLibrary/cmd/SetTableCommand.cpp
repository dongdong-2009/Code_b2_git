/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTableCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/SetTableCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetTableCommand::SetTableCommand(CommandOwner * owner)
: LeafCommand(owner, "table")
{
}

bool SetTableCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 3)
    {
        err << "ERROR: insufficient parameters. Expected at least 3, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    //
    // determine the table
    std::string tablename = args.front();
    args.pop_front();
    TableIdentifier tableId = ATSSimUtility::string2TableIdentifier(tablename);
    if (tableId==INVALID_IDENTIFIER)
    {
        err << "ERROR: invalid table name specified: " << tablename << std::endl;
        return false;
    }
	
    //
    // table address
    // At the stations, the Ats tables are shared, and have location STATION
    // rather than MSS|SS1|SS2.
    if ( (tableId >= AtsIdentifierLo) && (tableId <= AtsIdentifierHi) )
    {
        if ( (location==MSS) || (location==SS1) || (location==SS2) )
        {
            location = STATION;
        }
    }

	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table " << tablename << ", id=" << tableId << std::endl;
        return false;
	}
    query->setTableAddress(tAddr);

    //
	// the record number
    std::string recordstr = args.front();
    args.pop_front();
    bool ok = false;
    unsigned long record = query->str2ul(recordstr.c_str(), err, ok);
    if ( ! ok )
    {
        //TODO log a warning
        err << "WARNING: could not convert value to record number: " << recordstr << std::endl;
        return false;
    }
    int numrecords = query->setRecord(record);

    // copy the remaining parameters to the query parameter list
    int numgoodargs = query->parseArgs(args,err);
    if ( numgoodargs < 1 )
    {
        err << "ERROR: no valid arguments" << std::endl;
        return false;
    }

    return true;
}


void SetTableCommand::usage(const std::string & name, std::ostream & os) const
{
    os << "table <table name> <record number> <field=value> [field=value] ..." << std::endl;
}

