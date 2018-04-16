/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/SetWatchdogCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/SetWatchdogCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWatchdog.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetWatchdogCommand::SetWatchdogCommand(CommandOwner * owner)
: LeafCommand(owner, "watchdog")
{
}

bool SetWatchdogCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() != 1)
    {
        err << "ERROR: incorrect parameters. Expected 1, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsWatchdog;
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

    // don't parse for a record number, that would be silly
    int numrecords = query->setRecord(1);

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

            if (fieldstr == "value")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(ATSSimTableAtsWatchdog::getValueFieldNumber(), value);
                }
            }
            else if (fieldstr == "offset")
            {
                UserQuery::FieldValueType value = query->str2FieldValueType( valuestr.c_str(), err, ok );
                if ( ok )
                {
                    query->pushArg(ATSSimTableAtsWatchdog::getOffsetFieldNumber(), value);
                }
            }
            else
            {
                err << "WARNING: unrecognised \"set watchdog\" argument: " << fieldstr << std::endl;
            }
        }
        else if (parameters.size() == 1)
        {
            if (s == "pause")
            {
                query->pushArg(ATSSimTableAtsWatchdog::getPauseResumeFieldNumber(), 1);
            }
            else if (s == "resume")
            {
                query->pushArg(ATSSimTableAtsWatchdog::getPauseResumeFieldNumber(), 0);
            }
            else
            {
                err << "WARNING: unrecognised \"set watchdog\" argument: " << s << std::endl;
            }
        }
        else
        {
            //TODO log a warning ?
            err << "WARNING: too many equals signs ('=') in argument: " << s << std::endl;
        }

        argiter++;
    }
    return true;
}


void SetWatchdogCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " [pause|resume] [offset=n] [value=n]" << std::endl;
}

