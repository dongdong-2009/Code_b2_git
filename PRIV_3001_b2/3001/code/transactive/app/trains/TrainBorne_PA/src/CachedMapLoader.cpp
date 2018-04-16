/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/pa/PAManager/src/CachedMapLoader.cpp $
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

#include "StdAfx.h"
#include "app/trains/TrainBorne_PA/src/CachedMapLoader.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

namespace CachedMapLoader
{
    void loadAllCachedMaps()
    {
        refreshKeyToTrainDvaMessageRecordMap();

        // Tell the online update listener we want to automatically register
        // any new keys as they come through
        TA_Base_Core::OnlineUpdateListener::getInstance().setListenOutForNewKeys(true);

        // Pa Train DVA Messages listened out for
        TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().startListening();
    }
    
    void refreshKeyToTrainDvaMessageRecordMap()
    {
        FUNCTION_ENTRY("refreshKeyToTrainDvaMessageRecordMap");

        // Want to load all data for these items, regardless of location        
        try
        {
            // Attempt to retrieve from agent
            TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* ret = NULL;
            CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), getTrainDvaMessageRecords, () );
            std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords> messages( ret );

            // Clear all entries before refreshing items
            TA_IRS_Bus::PaTrainDvaMessageCachedMap& cachedMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap();

            cachedMap.clearKeyToRecordMap();

            for (unsigned int i = 0; i < messages->length(); i ++)
            {
                const TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecord& msg = (*messages)[i];

                TA_IRS_Bus::TrainDvaMessageRecord record;

		        record.m_pKeyOrId   = msg.m_pKeyOrId;
		        record.m_label      = msg.m_label.in();		       
                                
                // Insert the items directly into cached map
                cachedMap.updateRecordMapWithItem(record.m_pKeyOrId, record);
            }

            // Must tell the cached map it's been initialised, else it will assert
            cachedMap.flagAsInitialised();
        }
        catch (...)
        {
            // Fall back to direct DB access on failure
            TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToTrainDvaMessageRecordMap();
        }

        FUNCTION_EXIT;
    }

}   // namespace CachedMapLoader
