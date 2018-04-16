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

#if !defined(RadioGroupUtils_H)
#define RadioGroupUtils_H

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"

namespace TA_IRS_Bus
{
    
    namespace RadioGroupTypes
    {
        
        /**
         * This is a simple utility class to work out group conflicts.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:51:01 PM
         */
        class RadioGroupUtils
        {
        public:

            /**
             * This will return true if the given group is locked. It will check if the group
             * itself is locked, and also if any 'overlapping' groups are locked.
             * 
             * @return true if the group is locked. False if it can be used or if the group is
             * not known
             * 
             * @param groupTsi    The TSI to check
             * @param groupList    The list of all radio groups.
             */
            static bool isGroupLocked( const std::string& groupTsi, const TrainRadioGroupMap& groupList );


            /**
             * Given all groups, get only the ones available for use.
             * 
             * @return a list of groups that can be used.
             * 
             * @param groupMap    The list of all groups, or all groups for the given location
             */
            static TrainRadioGroupMap getAvaliableGroups( const TrainRadioGroupMap& groupMap );


            /**
             * Given all available groups (ie from getAvailableGroups), get only the ones
             * available for use covering the given locations.
             * 
             * @return a list of groups that can be used. to cover the given locations
             * 
             * @param groupMap    The list of all groups, or all groups for the given location
             * @param locations    the locations that must be covered
             */
            static TrainRadioGroupMap filterRadioGroupsByLocation( const TrainRadioGroupMap& groupMap, 
                                                                   GroupLocationList& locations );

        private:
            
            RadioGroupUtils();
            ~RadioGroupUtils();
        };

    }

}
#endif // !defined(RadioGroupUtils_H)
