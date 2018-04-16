/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/CachedMapLoader.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
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
    void refreshKeyToTrainDvaMessageRecordMap();

}   // namespace CachedMapLoader