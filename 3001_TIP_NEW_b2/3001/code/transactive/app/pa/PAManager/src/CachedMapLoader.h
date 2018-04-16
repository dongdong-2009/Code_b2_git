/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/CachedMapLoader.h $
  * @author Jade Lee
  * @version $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/03/09 17:49:19 $
  * Last modified by: $Author: huang.wenbo $
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
    void loadAllCachedMaps();
    
    /**
     * refreshKeyToStationDvaMessageRecordMap
     *
     * Refreshes contents of various cached maps
     *
     */
    void refreshKeyToStationDvaMessageRecordMap(unsigned int locationKey);
    void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey);
    void refreshKeyToPaZoneRecordMap(unsigned long locationKey = 0);
    void refreshKeyToDvaVersionRecordMap(unsigned long locationKey = -1);

}   // namespace CachedMapLoader