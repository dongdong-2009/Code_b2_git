/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/ShowStationCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Definition/Implementation file for PASimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowStationCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimulatorCommandProcessor.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;


ShowStationCommand::ShowStationCommand(CommandOwner * owner, PASimulatorCommandProcessor * commandProcessor)
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
        err << "Current location is " << PASimUtility::location2string(location) << std::endl;
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

