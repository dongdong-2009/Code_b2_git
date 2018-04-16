/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/LeafCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/LeafCommand.h"

// #include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;

LeafCommand::LeafCommand(CommandOwner * owner, const char * name)
: HierarchicalCommand(owner, name)
{
}

bool LeafCommand::isCommand(const std::string & name)
{
    return (name == getName());
}

bool LeafCommand::hasSubCommand(const std::string & name)
{
    return false;
}


LocationType LeafCommand::mergeStations(TableIdentifier tableId, LocationType location)
{
    LocationType newLocation=location;
    //
    // At the stations, the PA tables are shared, and have location STATION
    // rather than MSS|SS1|SS2.
    if ( (tableId >= PAIdentifierLo) && (tableId <= PAIdentifierHi) )
    {
        if ( (location==MSS) || (location==SS1) || (location==SS2) )
        {
            newLocation = STATION;
        }
    }

    return newLocation;
}


bool LeafCommand::parseTime
(   const std::string & s,
    Word & x_hour,
    Word & x_minute,
    Word & x_second,
    std::ostream & err)
{
    if ( s.length() != 6 )
    {
        err << "ERROR: time string wrong length. Expected 6 characters, got "
            << s.length() << ": " << s << std::endl;
        return false;
    }

    for ( int idx=0 ; idx<6 ; idx++ )
    {
        if ( (s[idx] < '0') || (s[idx] > '9') )
        {
            err << "ERROR: time string contains non-digit characters: "
                << s << std::endl;
            return false;
        }
    }

    char substr[3];
    substr[2] = '\0';

    substr[0] = s[0];
    substr[1] = s[1];
    x_hour = atoi(substr);

    substr[0] = s[2];
    substr[1] = s[3];
    x_minute = atoi(substr);

    substr[0] = s[4];
    substr[1] = s[5];
    x_second = atoi(substr);

    return true;
}

bool LeafCommand::parseDate
(   const std::string & s,
    Word & x_year,
    Word & x_month,
    Word & x_day,
    std::ostream & err)
{
    size_t slen = s.length();
    if ( ( slen != 6 ) && ( slen != 8 ) )
    {
        err << "ERROR: date string wrong length. Expected 6 or 8 characters, got "
            << slen << ": " << s << std::endl;
        return false;
    }

    for ( int idx=0 ; idx<slen ; idx++ )
    {
        if ( (s[idx] < '0') || (s[idx] > '9') )
        {
            err << "ERROR: date string contains non-digit characters: " << s << std::endl;
            return false;
        }
    }


    if (slen==6)
    {
        char substr[3];
        substr[2] = '\0';
        substr[0] = s[0];
        substr[1] = s[1];
        x_year = atoi(substr);

        substr[0] = s[2];
        substr[1] = s[3];
        x_month = atoi(substr);

        substr[0] = s[4];
        substr[1] = s[5];
        x_day = atoi(substr);
    }
    else if (slen==8)
    {
        char substr[5];
        strncpy(substr,s.c_str(),4);
        substr[4] = '\0';
        x_year = atoi(substr);

        substr[0] = s[4];
        substr[1] = s[5];
        substr[2] = '\0';
        x_month = atoi(substr);

        substr[0] = s[6];
        substr[1] = s[7];
        x_day = atoi(substr);
    }

    if (x_year<100)
    {
        x_year += 2000;
    }
    else if (x_year<2000)
    {
        err << "ERROR: date year out of range. Should be 2000<=year<=2100, got:" << x_year << std::endl;
        return false;
    }

    return true;
}

