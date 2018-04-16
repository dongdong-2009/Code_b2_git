/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/cmd/SetDssCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/SetDssCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetDssCommand::SetDssCommand(CommandOwner * owner)
: LeafCommand(owner, "dss")
{
}


bool SetDssCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 2)
    {
        err << "ERROR: insufficient parameters. Expected at least 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsDSS;
    location = mergeStations(tableId, location);
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"AtsDss\" (ouch)" << std::endl;
        return false;
	}
    query->setTableAddress(tAddr);

    //
	// the record number
    std::string recordstr = args.front(); args.pop_front();
    bool ok = false;
    unsigned long record = query->str2ul(recordstr.c_str(), err, ok);
    if ( ! ok )
    {
        //TODO log a warning
        err << "WARNING: could not convert value to record number: " << recordstr << std::endl;
        return false;
    }
    int numrecords = query->setRecord(record);

    std::string s;
    TokenList parameters;
    TokenList::const_iterator argiter = args.begin();

    while ( argiter != args.end() )
    {
        bool ok = false;
        s = *argiter;
        parameters.clear();
        ATSSimUtility::tokenise(s.c_str(), parameters, "=");

        if (parameters.size() == 2)
        {

            std::string fieldstr = parameters.front();
            parameters.pop_front();

            std::string valuestr = parameters.front();
            parameters.pop_front();

            if (fieldstr == "time")
            {
                Word hour, minute, second;
                ok = parseTime( valuestr, hour, minute, second, err);
                if ( ok )
                {
                    query->pushArg(6, hour);
                    query->pushArg(8, minute);
                    query->pushArg(10, second);
                }
            }
            else if (fieldstr == "date")
            {
                Word year, month, day;
                ok = parseDate( valuestr, year, month, day, err);
                if ( ok )
                {
                    query->pushArg(0, year);
                    query->pushArg(2, month);
                    query->pushArg(4, day);
                }
            }
            else if (fieldstr == "event")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(12, value);
                }
            }
            else if (fieldstr == "inputhigh")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(14, value);
                }
            }
			else if (fieldstr == "inputlow")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(16, value);
                }
            }
            else
            {
                err << "ERROR: unrecognised set dss argument: " << valuestr << std::endl;
            }
        }
        else
        {
            //TODO log a warning
            err << "WARNING: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
        }

        argiter++;
    }
    return true;
}


void SetDssCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> [time=HHMMSS] [date=YYMMDD] [event=n] [inputhigh=n] [inputlow=n]" << std::endl;
}

