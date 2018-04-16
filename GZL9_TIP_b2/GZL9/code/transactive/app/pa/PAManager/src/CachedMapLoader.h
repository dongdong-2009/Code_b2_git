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

#pragma once

namespace CachedMapLoader
{
    /**
     * loadAllCachedMaps
     *
     * Loads all cached maps required for PA Manager
     *
     */
    void loadAllCachedMaps( unsigned long ulLocationKey, bool bIsOcc = false );

    /**
     * refreshKeyToStationDvaMessageRecordMap
     *
     * Refreshes contents of various cached maps
     *
     */
    void refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey);
    void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey);
    void refreshKeyToPaZoneRecordMap(unsigned long locationKey = -1);

}   // namespace CachedMapLoader