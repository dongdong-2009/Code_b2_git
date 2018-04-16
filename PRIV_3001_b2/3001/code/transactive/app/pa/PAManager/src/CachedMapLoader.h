/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/CachedMapLoader.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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