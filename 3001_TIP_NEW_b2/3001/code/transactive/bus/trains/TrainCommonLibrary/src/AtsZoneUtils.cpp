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

#include "bus/trains/TrainCommonLibrary/src/AtsZoneUtils.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <algorithm>
#include <math.h>

namespace TA_IRS_Bus
{
       
    void AtsZoneUtils::getCctvConfiguration( AtsZoneUtils::LocationToCctvZoneMap& locationToZoneMap,
                                             AtsZoneUtils::CctvZoneToLocationMap& zoneToLocationMap )
    {
        FUNCTION_ENTRY("getCctvConfiguration");

        // load all ATS fixed block records
        TA_IRS_Core::IAtsFbMapRecords allRecords = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAllAtsFbMapRecords();


        // create a map of each zone to its branch id list, and each location to its branch id list
        // and also the reverse mapping
        typedef std::map< unsigned long, LocationList > IdMap;
        typedef IdMap::iterator IdMapIt;

        // location to the branches it contains
        IdMap locationToBranchList;

        // cctv zone to the branches it contains
        IdMap cctvZoneToBranchList;

        // each branch to the locations it contains
        IdMap branchToLocationList;

        // each branch to the cctv zones it contains
        IdMap branchToCctvZoneList;


        for ( TA_IRS_Core::IAtsFbMapRecordsIt recordIter = allRecords.begin();
              allRecords.end() != recordIter; ++recordIter )
        {
            // get the branch ID
            unsigned long branchId = ( *recordIter )->getBranchId();

            // get the zone ID - but remove conflict and pre conflict markers
            unsigned long zoneId = static_cast<unsigned long> ( fabs( ( *recordIter )->getZoneId() ) );

            // get the zone type
            std::string zoneType = ( *recordIter )->getZoneTypeName();

            if ( TA_IRS_Core::AtsFixedBlockMapAccessFactory::RadioZoneType == zoneType )
            {
                // location key to branch list
                locationToBranchList[ zoneId ].insert( branchId );

                // branch id to location list
                branchToLocationList[ branchId ].insert( zoneId );
            }
            else if ( TA_IRS_Core::AtsFixedBlockMapAccessFactory::CctvZoneType == zoneType )
            {
                // cctv zone to branch list
                cctvZoneToBranchList[ zoneId ].insert( branchId );

                // branch id to cctv zone list
                branchToCctvZoneList[ branchId ].insert( zoneId );
            }

            delete ( *recordIter );

            ( *recordIter ) = NULL;
        }

        allRecords.clear();

        if ( true == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DEBUG_ZONE_CONFIG ) )
        {
            
            IdMapIt debugIter;
        
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "locationToBranchList" );

            for ( debugIter = locationToBranchList.begin();
                  locationToBranchList.end() != debugIter; ++debugIter )
            {
                std::ostringstream debugStr;
                debugStr << "    Location: " << debugIter->first << " Branches: ";

                for ( LocationList::iterator locIter = debugIter->second.begin();
                      debugIter->second.end() != locIter; ++locIter )
                {
                    debugStr << (*locIter) << " ";
                }

                // these can be long, use internal operation to log big strings
                TA_Base_Core::DebugUtil::getInstance().logLargeString(
                    __FILE__, __LINE__,
                    TA_Base_Core::DebugUtil::DebugDebug,
                    debugStr.str() );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "branchToLocationList" );
            for ( debugIter = branchToLocationList.begin();
                  branchToLocationList.end() != debugIter; ++debugIter )
            {
                std::ostringstream debugStr;
                debugStr << "    Branch: " << debugIter->first << " Locations: ";

                for ( LocationList::iterator locIter = debugIter->second.begin(); debugIter->second.end() != locIter; ++locIter )
                {
                    debugStr << (*locIter) << " ";
                }

                // these can be long, use internal operation to log big strings
                TA_Base_Core::DebugUtil::getInstance().logLargeString(
                    __FILE__, __LINE__,
                    TA_Base_Core::DebugUtil::DebugDebug,
                    debugStr.str() );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "cctvZoneToBranchList" );
            for ( debugIter = cctvZoneToBranchList.begin();
                  cctvZoneToBranchList.end() != debugIter; ++debugIter )
            {
                std::ostringstream debugStr;
                debugStr << "    Zone: " << debugIter->first << " Branches: ";

                for ( LocationList::iterator locIter = debugIter->second.begin(); debugIter->second.end() != locIter; ++locIter )
                {
                    debugStr << (*locIter) << " ";
                }

                // these can be long, use internal operation to log big strings
                TA_Base_Core::DebugUtil::getInstance().logLargeString(
                    __FILE__, __LINE__,
                    TA_Base_Core::DebugUtil::DebugDebug,
                    debugStr.str() );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "branchToCctvZoneList" );
            for ( debugIter = branchToCctvZoneList.begin();
                  branchToCctvZoneList.end() != debugIter; ++debugIter )
            {
                std::ostringstream debugStr;
                debugStr << "    Branch: " << debugIter->first << " Zones: ";

                for ( LocationList::iterator locIter = debugIter->second.begin(); debugIter->second.end() != locIter; ++locIter )
                {
                    debugStr << (*locIter) << " ";
                }

                // these can be long, use internal operation to log big strings
                TA_Base_Core::DebugUtil::getInstance().logLargeString(
                    __FILE__, __LINE__,
                    TA_Base_Core::DebugUtil::DebugDebug,
                    debugStr.str() );
            }
        }
        
        // TODO, possibly
        // There may be a need to remove duplicate branch IDs,
        // ie branch IDs that appear in more than one cctv zone or location.
        // it depends on the actual configuration and if having these duplicates causes problems

        
        // now here we have a list of each zone, and the branches within for radio and cctv zones
        // and a map from each branch id to the locations/zones it is in

        // the easiest thing to build first is the map of cctv zones to locations they cover
        for ( IdMapIt cctvZoneToBranchListIter = cctvZoneToBranchList.begin();
              cctvZoneToBranchList.end() != cctvZoneToBranchListIter; ++cctvZoneToBranchListIter )
        {
            bool foundMatch = false;

            // for each cctv zone, for each branch in that zone
            for ( LocationList::iterator branchListIter = cctvZoneToBranchListIter->second.begin();
                  cctvZoneToBranchListIter->second.end() != branchListIter; ++branchListIter )
            {
                // find the list of locations for that branch
                IdMapIt findIter = branchToLocationList.find( *branchListIter );
                if ( branchToLocationList.end() != findIter )
                {
                    // insert each location into the zoneToLocationMap
                    // this is the proper way:
                    //zoneToLocationMap[ cctvZoneToBranchListIter->first ].insert( findIter->second.begin(),
                    //                                                             findIter->second.end() );

                    // however VC++ cant distinguish between an unsigned long and an iterator to an unsigned long...

                    for ( LocationList::iterator matchingZoneIter = findIter->second.begin();
                          findIter->second.end() != matchingZoneIter; ++matchingZoneIter )
                    {
                        zoneToLocationMap[ cctvZoneToBranchListIter->first ].insert( *matchingZoneIter );
                    }

                    foundMatch = true;
                }
            }

            if ( true == foundMatch )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Mapped CCTV Zone: %d to locations: %s",
                             cctvZoneToBranchListIter->first,
                             listToString< LocationList >( zoneToLocationMap[ cctvZoneToBranchListIter->first ] ).c_str() );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "CCTV Zone %d does not map to any locations",
                             cctvZoneToBranchListIter->first );
            }
        }

        // at this point, there is a map of cctv zones to the locations covered by that zone
        // it is possible that a location is in multiple cctv zones
        // see the comment above about removing duplicate branch ids


        // now it is time to build a map of locations to which cctv zone covers them
        for ( IdMapIt locationToBranchListIter = locationToBranchList.begin();
              locationToBranchList.end() != locationToBranchListIter; ++locationToBranchListIter )
        {
            // for each location
            
            // find each zone in that location, and the number of times it appears
            // the majority wins approach will be taken
            std::map< unsigned long, unsigned long > matchingZonesAndCount;

            // for each branch in that location
            for ( LocationList::iterator branchListIter = locationToBranchListIter->second.begin();
                  locationToBranchListIter->second.end() != branchListIter; ++branchListIter )
            {   
                // find the list of zones for that branch
                IdMapIt findIter = branchToCctvZoneList.find( *branchListIter );
                if ( branchToCctvZoneList.end() != findIter )
                {
                    // there are 1 or more cctv zones for this branch
                    for ( LocationList::iterator matchingZoneIter = findIter->second.begin();
                          findIter->second.end() != matchingZoneIter; ++matchingZoneIter )
                    {
                        // see if this is a new entry in the matching zone map
                        std::map< unsigned long, unsigned long >::iterator matchingZonesAndCountIter =
                            matchingZonesAndCount.find( *matchingZoneIter );
                        if ( matchingZonesAndCount.end() != matchingZonesAndCountIter )
                        {
                            ++(matchingZonesAndCountIter->second);
                        }
                        else
                        {
                            matchingZonesAndCount[ *matchingZoneIter ] = 1;
                        }
                    }
                }
            }

            // now each location has none, one or more matching cctv zones
            if ( 0 == matchingZonesAndCount.size() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Location %d does not have a corresponding CCTV Zone",
                             locationToBranchListIter->first );
            }
            else if ( 1 == matchingZonesAndCount.size() )
            {
                locationToZoneMap[ locationToBranchListIter->first ] = matchingZonesAndCount.begin()->first;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Location %d maps to CCTV Zone %d",
                             locationToBranchListIter->first,
                             matchingZonesAndCount.begin()->first );
            }
            else
            {
                // there are multiple matches
                // take the one with the most hits
                unsigned long count = 0;
                for ( std::map< unsigned long, unsigned long >::iterator matchingZonesAndCountIter = matchingZonesAndCount.begin();
                      matchingZonesAndCount.end() != matchingZonesAndCountIter; ++matchingZonesAndCountIter )
                {
                    if ( matchingZonesAndCountIter->second > count )
                    {
                        locationToZoneMap[ locationToBranchListIter->first ] = matchingZonesAndCountIter->first;

                        count = matchingZonesAndCountIter->second;
                    }
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Location %d maps to CCTV Zone %d",
                             locationToBranchListIter->first,
                             locationToZoneMap[ locationToBranchListIter->first ] );
            }
        }

        FUNCTION_EXIT;
    }


}

