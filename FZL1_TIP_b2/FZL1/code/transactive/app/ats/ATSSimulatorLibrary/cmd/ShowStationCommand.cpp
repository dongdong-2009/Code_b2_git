/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/ShowStationCommand.cpp $
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

#include "app/ats/ATSSimulatorLibrary/cmd/ShowStationCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimulatorCommandProcessor.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;


ShowStationCommand::ShowStationCommand(CommandOwner * owner, ATSSimulatorCommandProcessor * commandProcessor)
: LeafCommand(owner, "station")
, m_commandProcessor(commandProcessor)
{
}

bool ShowStationCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking. args.size() includes the 'mode' word
    if (args.size() > 1)
    {
        err << "WARNING: too many parameters. Expected 0 or 1, got "
            << args.size() << ": " << argstr << std::endl;
    }
    
    bool ok=true;
    if ( args.size() == 1 )
    {
        std::string locationId = args.front();
        args.pop_front();

        LocationType location = TableAddressRegister::getInstance().findLocation(locationId);
        if ( location != NO_LOCATION )
        {
            if ( (location==MSS) || (location==SS1) || (location==SS2) )
            {
                m_commandProcessor->setShowLocation(location);
            }
            else
            {
                err << "ERROR: setting a location with 'show station ...' only valid at stations" << std::endl;
                ok=false;
            }
        }
        else
        {
            err << "ERROR: Could not find location=" << locationId << std::endl;
            ok=false;
        }
    }

    if (ok)
    {
        LocationType location = m_commandProcessor->getShowLocation();
        err << "Current location is " << ATSSimUtility::location2string(location) << std::endl;
    }
    else
    {
        std::list< std::string > locationIds;
        TableAddressRegister::getInstance().getLocationIdentifiers(MSS, locationIds);
        TableAddressRegister::getInstance().getLocationIdentifiers(SS1, locationIds);
        TableAddressRegister::getInstance().getLocationIdentifiers(SS2, locationIds);
        err << "Possible location identifiers:" ;
        for ( std::list< std::string >::iterator iter = locationIds.begin() ;
                iter != locationIds.end() ;
                iter++
            )
        {
            if ( iter!=locationIds.begin() )
            {
                err << ", ";
            }
            err << (*iter);
        }
        err << std::endl;
    }

    return ok;
}


void ShowStationCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " [location identifier]" << std::endl;
}

