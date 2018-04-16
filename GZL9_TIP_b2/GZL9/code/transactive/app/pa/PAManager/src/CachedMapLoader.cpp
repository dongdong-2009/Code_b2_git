/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * Manages loading of cached maps (needs to be centralized, to allow the PA Agent
  * to be used to load cached map data where possible, rather than direct DB access)
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/CachedMapLoader.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"


namespace CachedMapLoader
{
    void loadAllCachedMaps( unsigned long ulLocationKey, bool bIsOcc )
    {
        // Only want to load up location specific data for these, for all locations
        refreshKeyToStationDvaMessageRecordMap(ulLocationKey);
        
        refreshKeyToPaZoneGroupRecordMap(ulLocationKey);

        if ( bIsOcc )
        {
            // OCC should load all zones
		    refreshKeyToPaZoneRecordMap();
        }
        else
        {
		    refreshKeyToPaZoneRecordMap(ulLocationKey);
        }
    }

    void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey)
    {
        // Zone groups are not loaded by PA Agent, must load directly from DB
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneGroupRecordMap(locationKey);
    }

    void refreshKeyToPaZoneRecordMap(unsigned long locationKey)
    {
        FUNCTION_ENTRY("refreshKeyToPaZoneRecordMap");

        // Fall back to direct DB access on failure
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneRecordMap(locationKey);

        FUNCTION_EXIT;
    }

    void refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey)
    {   
        FUNCTION_ENTRY("refreshKeyToStationDvaMessageRecordMap");

        // If there is any kind of failure, simply fall back to direct DB access
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToStationDvaMessageRecordMap(locationKey);

        FUNCTION_EXIT;
    }

}   // namespace CachedMapLoader
