/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetTwpCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTwpCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;

SetTwpCommand::SetTwpCommand(CommandOwner * owner)
: LeafCommand(owner, "twp")
{
}

bool SetTwpCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 2)
    {
        err << "ERROR: insufficient parameters. Expected at least 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = PAWash;
    //
    // Train Wash Plant table only valid at DEPOT
    if ( location != DEPOT )
    {
        err << "ERROR: PA Train Wash Plant table only valid at DEPOT" << std::endl;
        return false;
    }
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"PATrain\" (ouch)" << std::endl;
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
        PASimUtility::tokenise(s.c_str(), parameters, "=");

        if (parameters.size() == 2)
        {

            std::string fieldstr = parameters.front();
            parameters.pop_front();

            std::string valuestr = parameters.front();
            parameters.pop_front();

            unsigned short field = 0xFFFF;

            if (fieldstr=="validity")
            {
                field = 0;
            }
            else if (fieldstr=="no_wash")
            {
                field = 1;
            }
            else if (fieldstr=="daily_rinse")
            {
                field = 2;
            }
            else if (fieldstr=="daily_detergent")
            {
                field = 3;
            }
            else if (fieldstr=="weekly_wash")
            {
                field = 4;
            }
            else if (fieldstr=="cab_end_wash")
            {
                field = 5;
            }
            else if (fieldstr=="stop")
            {
                field = 6;
            }
            else if (fieldstr=="restart")
            {
                field = 7;
            }
            else
            {
                field = query->str2ul( valuestr.c_str(), err, ok );
                if ( (!ok) || (field>47) || (field<36) )
                {
                    err << "ERROR: invalid train wash plant field: " << fieldstr << std::endl;
                    return false;
                }
            }

            if (field != 0xFFFF)
            {
                UserQuery::FieldValueType value = PASimUtility::string2bool(valuestr);
                query->pushArg(field, value);
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


void SetTwpCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " 1 <field=value> [field=value]..." << std::endl;
}

