/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/SetWildCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/SetWildCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetWildCommand::SetWildCommand(CommandOwner * owner)
: LeafCommand(owner, "wild")
{
}

bool SetWildCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 2)
    {
        err << "ERROR: insufficient parameters. Expected at least 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsWILD;
    //
    // In order to correctly fetch the table address
    // At the stations, the Ats tables are shared, and have location STATION
    // rather than MSS|SS1|SS2.
    if ( location!=OCC )
    {
        err << "ERROR: WILD table only valid at Occ" << std::endl;
        return false;
    }
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"AtsWILD\" (ouch)" << std::endl;
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
            else if (fieldstr == "train")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(12, value);
                }
            }
            else if (fieldstr == "cab")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(14, value);
                }
            }
            else if (fieldstr == "hcdir")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(17, value);
                }
                else if ( valuestr=="unknown" )
                {
                    query->pushArg(17, 0);
                }
                else if ( valuestr=="down" )
                {
                    query->pushArg(17, 1);
                }
                else if ( valuestr=="up" )
                {
                    query->pushArg(17, 2);
                }
                else
                {
                    err << "ERROR: unrecognised head cab direction: " << valuestr << std::endl;
                }
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


void SetWildCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> [time=HHMMSS] [date=YYMMDD] [train=n] [cab=n] [hcdir=unknown|up|down|0..2]" << std::endl;
}

