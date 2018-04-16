#ifndef LeafCommand_H
#define LeafCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/LeafCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for classes used by ATSSimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{

    /** Leaf node "base" class.  Of the command hierarchy, that is.
      * Leaves makeQuery() and usage() pure for concrete subclasses.
      */
    class LeafCommand : public HierarchicalCommand
    {
    public:
        LeafCommand(CommandOwner * owner, const char * name);

        bool isCommand(const std::string & name);

        bool hasSubCommand(const std::string & name);

    protected:
        //
        // Useful functions for leaf commands

        /** At the stations, the Ats tables are shared, and have location
          * STATION rather than MSS|SS1|SS2. This function uses a tableId
          * and current location to merge the three station locations into
          * the merged STATION, IFF necessary.
          *
          * @param tableId the table identifier
          * @param location the current location type
          * @return the appropriately merged station location
          */
        LocationType mergeStations(TableIdentifier tableId, LocationType location);

        /** parses a 6 character string as obtained by a time=HHMMSS field/value
          * pair into 3 unsigned short integers representing the hour, minute
          * and second values
          *
          * @param s the string to parse
          * @param x_hour the returned hour
          * @param x_minute the returned minute
          * @param x_second the returned second
          * @param os the ostream to which to print error explanations
          * @return true if the parse succeeded, false otherwise
          */
        bool parseTime(const std::string & s, Word & x_hour, Word & x_minute, Word & x_second, std::ostream & os);

        /** parses a 6 or 8 character string as obtained by a time=YYYYmmdd
          * field/value pair into 3 unsigned short integers representing the
          * year, month and day values. If the year is less than 100, it is
          * made to be between 2000 and 2100, as per the SIDD RevB
          *
          * @param s the string to parse
          * @param x_year the returned year
          * @param x_month the returned month
          * @param x_day the returned day
          * @param os the ostream to which to print error explanations
          * @return true if the parse succeeded, false otherwise
          */
        bool parseDate(const std::string & s, Word & x_year, Word & x_month, Word & x_day, std::ostream & os);
    }; // class LeafCommand


} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef LeafCommand_H
