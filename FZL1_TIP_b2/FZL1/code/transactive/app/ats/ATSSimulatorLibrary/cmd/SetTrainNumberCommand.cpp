/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTrainNumberCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainNumberCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetTrainNumberCommand::SetTrainNumberCommand(CommandOwner * owner)
: LeafCommand(owner, "number")
{
}

bool SetTrainNumberCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 2)
    {
        err << "ERROR: insufficient parameters. Expected at least 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsTrain;
    //
    // In order to correctly fetch the table address
    // At the stations, the Ats tables are shared, and have location STATION
    // rather than MSS|SS1|SS2.
    if ( (location==MSS) || (location==SS1) || (location==SS2) )
    {
        location = STATION;
    }
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"AtsTrain\" (ouch)" << std::endl;
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

            if (fieldstr == "physical")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(0, value);
                }
            }
            else if (fieldstr == "atc")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(2, value);
                }
            }
            else if (fieldstr == "service")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(6, value);
                }
            }
            else if (fieldstr == "schedule")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(8, value);
                }
            }
            else
            {
                err << "ERROR: unrecognised train number field: " << fieldstr << std::endl;
            }
        }
        else
        {
            //TODO log a warning ?
            err << "WARNING: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
        }

        argiter++;
    }
    return true;
}


void SetTrainNumberCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> [schedule=sch] [service=svc] [atc=a] [physical=p]" << std::endl;
}

