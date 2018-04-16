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

#include <algorithm>
#include "bus/trains/TrainCommonLibrary/src/RadioGroupUtils.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    
    namespace RadioGroupTypes
    {
        
        RadioGroupUtils::RadioGroupUtils(){}

        RadioGroupUtils::~RadioGroupUtils(){}


        bool RadioGroupUtils::isGroupLocked( const std::string& groupTsi, const TrainRadioGroupMap& groupList )
        {
            FUNCTION_ENTRY("isGroupLocked");
            
            // find the group in the given map
            TrainRadioGroupMap::const_iterator findIter = groupList.find( groupTsi );

            // if it is not in the list, return false
            if ( findIter == groupList.end() )
            {
                // log an info message indicating this group is not found
                LOG_GENERIC( SourceInfo, 
                             TA_Base_Core::DebugUtil::DebugInfo, 
                             "group %s is not locked because it is not in the list of radio groups",
                             groupTsi.c_str() );

                FUNCTION_EXIT;
                return false;
            }

            if ( true == findIter->second.locked )
            {
                // log an info message indicating this group is not found
                LOG_GENERIC( SourceInfo, 
                             TA_Base_Core::DebugUtil::DebugInfo, 
                             "group %s is locked",
                             groupTsi.c_str() );

                FUNCTION_EXIT;
                return true;
            }
        
            // if it is in the map, iterate through the map, for each item
            for ( TrainRadioGroupMap::const_iterator iter = groupList.begin();
                  iter != groupList.end(); ++iter )
            {
                //   if the group is locked
                if ( true == iter->second.locked )
                {
                    // see if its locations intersect with the group found previously
                    // to see if the locations intersect, use std::set_intersection,
                    GroupLocationList tempList;
                    std::set_intersection( findIter->second.locationCoverage.begin(), findIter->second.locationCoverage.end(),
                                           iter->second.locationCoverage.begin(), iter->second.locationCoverage.end(),
                                           std::inserter( tempList, tempList.begin() ) );

                    // if they intersect, then the group is locked by another group,return true
                    if ( false == tempList.empty() )
                    {
                        // log an info message indicating this group is locked, 
                        // and the group that is causing it to be locked
                        LOG_GENERIC(SourceInfo, 
                                    TA_Base_Core::DebugUtil::DebugInfo, 
                                    "Group %s is locked indirectly because %s is locked", 
                                    groupTsi.c_str(), iter->second.groupName.c_str() );

                        FUNCTION_EXIT;
                        return true;
                    }                    
                }
             }

            FUNCTION_EXIT;
            return false;
        }


        TrainRadioGroupMap RadioGroupUtils::getAvaliableGroups(const TrainRadioGroupMap& groupMap)
        {
            FUNCTION_ENTRY("getAvaliableGroups");

            // create a GroupLocationList, to hold the locked locations
            GroupLocationList lockedLocations;
            
            // Go through the given map (groupList)
            TrainRadioGroupMap::const_iterator ite;
            for ( ite = groupMap.begin(); ite != groupMap.end(); ++ite )
            {
                if ( true == ite->second.locked )
                {
                    // if it is locked, add all its locations into lockedLocations
                    for ( GroupLocationList::const_iterator locIter = ite->second.locationCoverage.begin(); 
                          locIter != ite->second.locationCoverage.end(); ++locIter )
                    {
                        lockedLocations.insert( *locIter );
                    }
                }
            }

            // now there is a list of all locked location
            // all unlocked groups not covering these locations are available

            TrainRadioGroupMap availableGroups;

            for ( ite = groupMap.begin(); ite != groupMap.end(); ++ite )
            {
                 // if it is not locked
                if ( false == ite->second.locked )
                {
                    // use a std::set_intersection to determine if the location coverage
                    // intersects with the locked locations
                    GroupLocationList tempList;
                    std::set_intersection( ite->second.locationCoverage.begin(), ite->second.locationCoverage.end(),
                                           lockedLocations.begin(), lockedLocations.end(),
                                           std::inserter( tempList, tempList.begin() ) );
                    
                    // if they dont (the resulting set size is 0), then this group 
                    // is not locked, add this item to the availableGroups map
                    if ( true == tempList.empty() )
                    {
                        availableGroups.insert( TrainRadioGroupMap::value_type( ite->second.groupTsi, ite->second ) );
                    }
                }
            }

            FUNCTION_EXIT;
            return availableGroups;
        }


        TrainRadioGroupMap RadioGroupUtils::filterRadioGroupsByLocation( const TrainRadioGroupMap& groupMap,
                                                                         GroupLocationList& locations )
        {
            FUNCTION_ENTRY("filterRadioGroupsByLocation");
            
            // create a TrainRadioGroupMap (availableGroups)
            TrainRadioGroupMap availableGroups;

            // Go through the given map (groupList)
            for ( TrainRadioGroupMap::const_iterator ite = groupMap.begin();
                  ite != groupMap.end(); ++ite )
            {
                // see if it contains all the locations within locations
                // to acheive this, use the std::includes operation
                if ( std::includes( ite->second.locationCoverage.begin(), ite->second.locationCoverage.end(),
                                    locations.begin(), locations.end() ) == true )
                {
                    // if all the desired locations are covered, 
                    // add the group to the availableGroups map
                    availableGroups.insert( TrainRadioGroupMap::value_type( ite->second.groupTsi, ite->second ) );
                }

            }

            FUNCTION_EXIT;
            return availableGroups;
        }
        
    }
    
}
