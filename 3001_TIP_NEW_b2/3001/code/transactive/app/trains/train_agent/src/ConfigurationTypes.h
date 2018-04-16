/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(ConsoleInfo_H)
#define ConsoleInfo_H

#include "core/data_access_interface/src/ILocation.h"

#include <string>
#include <map>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:26 PM
     */
    struct ConsoleInfo
    {

        /**
         * The console key in the database
         */
        unsigned long consoleKey;


        /**
         * The location of the console
         */
        unsigned long location;


        /**
         * The origin to send in TIMS messages for commands issued by the operator at this console
         */
        unsigned char trainOrigin;

    };


    /**
     * Represents basic information about a location
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:26 PM
     */
    struct LocationInfo
    {

        /**
         * The key of this location
         */
        unsigned long key;


        /**
         * The name of this location
         */
        std::string name;


        /**
         * The type of this location
         */
        TA_Base_Core::ILocation::ELocationType type;


        /**
         * The location order on the track
         */
        unsigned long orderId;

    };


    /**
     * A map of location key to information
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:26 PM
     */
    typedef std::map< unsigned long, LocationInfo > LocationInfoMap;

} // TA_IRS_App

#endif // !defined(ConsoleInfo_H)
