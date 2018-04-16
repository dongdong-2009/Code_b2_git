/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/ShowIscsTableCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/ShowIscsTableCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;


ShowIscsTableCommand::ShowIscsTableCommand(CommandOwner * owner, const char * name)
: LeafCommand(owner, name)
{
}

bool ShowIscsTableCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() > 1)
    {
        err << "ERROR: too many parameters. Expected 0 or 1 , got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    //
    // determine the table
    TableIdentifier tableId = ATSSimUtility::string2TableIdentifier(getName(),"iscs");
    if (tableId==INVALID_IDENTIFIER)
    {
        err << "ERROR: invalid table name specified: " << getName() << std::endl;
        return false;
    }
    
    //
    // from which we figure the table address
    unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
    if (tAddr == TableAddressRegister::INVALID_ADDRESS)
    {
        err << "ERROR: could not determine table address for table " << getName() << ", id=" << tableId << std::endl;
        return false;
    }
    query->setTableAddress(tAddr);

    //
    // the [optional] records
    if ( ! args.empty() )
    {
        std::string recordstr = args.front();
        args.pop_front();
        int records = query->setRecords(recordstr, ULONG_MAX);
        if (records < 1)
        {
            err << "ERROR: invalid record range specified: " << recordstr << std::endl;
            return false;
        }
    }
    
    return true;
}


void ShowIscsTableCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " [record range]" << std::endl;
}

