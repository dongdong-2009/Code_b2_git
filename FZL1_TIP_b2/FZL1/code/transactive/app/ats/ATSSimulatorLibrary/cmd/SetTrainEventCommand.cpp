/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTrainEventCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainEventCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetTrainEventCommand::SetTrainEventCommand(CommandOwner * owner)
: LeafCommand(owner, "event")
{
}

bool SetTrainEventCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
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

            unsigned short field = 0xFFFF;

            if (fieldstr=="under_seat_fire_car1")
            {
                field = 36;
            }
            else if (fieldstr=="under_frame_fire_car1")
            {
                field = 37;
            }
            else if (fieldstr=="interior_smoke_car1")
            {
                field = 38;
            }
            else if (fieldstr=="exterior_smoke_car1")
            {
                field = 39;
            }
            else if (fieldstr=="under_seat_fire_car2")
            {
                field = 40;
            }
            else if (fieldstr=="under_frame_fire_car2")
            {
                field = 41;
            }
            else if (fieldstr=="interior_smoke_car2")
            {
                field = 42;
            }
            else if (fieldstr=="exterior_smoke_car2")
            {
                field = 43;
            }
            else if (fieldstr=="under_seat_fire_car3")
            {
                field = 44;
            }
            else if (fieldstr=="under_frame_fire_car3")
            {
                field = 45;
            }
            else if (fieldstr=="interior_smoke_car3")
            {
                field = 46;
            }
            else if (fieldstr=="exterior_smoke_car3")
            {
                field = 47;
            }
            else
            {
                field = query->str2ul( valuestr.c_str(), err, ok );
                if ( (!ok) || (field>47) || (field<36) )
                {
                    err << "ERROR: invalid train event field: " << fieldstr << std::endl;
                    return false;
                }
            }

            if (field != 0xFFFF)
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( (ok) && (value<5) ) // all smoke and fire detection values are <5
                {
                    query->pushArg(field, value);
                }
                else
                {
                    err << "ERROR: invalid train event value: " << fieldstr << std::endl;
                }
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


void SetTrainEventCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> <event=value> [event=value]..." << std::endl;
}

