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

#if !defined(AtsZoneUtils_H)
#define AtsZoneUtils_H

#include <map>
#include <set>

#define RPARAM_DEBUG_ZONE_CONFIG "DebugZoneConfiguration"

namespace TA_IRS_Bus
{
       
    /**
     * This is a simple utility class to work out ats zone configuration and intersection
     * @author adamr
     * @version 1.0
     * @created 13-Nov-2007 3:51:01 PM
     */
    class AtsZoneUtils
    {
    public:

        
        /**
         * Maps location keys to cctv zones
         */
        typedef std::map< unsigned long, unsigned long > LocationToCctvZoneMap;
        
        
        /**
         * Represents a list of locations
         */
        typedef std::set< unsigned long > LocationList;

        
        /**
         * Maps cctv zones to the locations covered by that zone
         */
        typedef std::map< unsigned long, LocationList > CctvZoneToLocationMap;

        
        /**
         * This will load ats zone configuration and correlate cctv zones and radio locations
         * 
         * @param locationToZoneMap The map of transactive locations to the cctv zone that covers them
         * @param zoneToLocationMap the map of cctv zones to the transactive locations they cover
         */
        static void getCctvConfiguration( LocationToCctvZoneMap& locationToZoneMap,
                                          CctvZoneToLocationMap& zoneToLocationMap );


    private:
        
        AtsZoneUtils();
        ~AtsZoneUtils();
    };


}
#endif // !defined(AtsZoneUtils_H)
