/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/CachedMapLoader.cpp $
  * @author Jade Lee
  * @version $Revision: #6 $
  *
  * Last modification: $DateTime: 2015/12/07 18:28:43 $
  * Last modified by: $Author: xiaoxia.li $
  * 
  * Manages loading of cached maps (needs to be centralized, to allow the PA Agent
  * to be used to load cached map data where possible, rather than direct DB access)
  *
  */

#include "StdAfx.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/TisCachedMaps.h"
#include "app/pa/PAManager/src/CachedMapLoader.h"
#include "app/pa/PAManager/src/CachedConfig.h"

using namespace TA_IRS_Bus;

namespace CachedMapLoader
{

    void loadAllCachedMaps()
    {
        unsigned long ulLocKey = CachedConfig::getInstance()->getAgentLocKey();
        unsigned long ulSchematicLocKey = CachedConfig::getInstance()->getSchematicLocKey();

        refreshKeyToStationDvaMessageRecordMap(ulLocKey);

        // Zone and zone group is load according to Schematic location
        if ( CachedConfig::getInstance()->isSchematicAtOcc() )
        {
            refreshKeyToPaZoneGroupRecordMap(ulLocKey);
            refreshKeyToPaZoneRecordMap();
        }
        else
        {
            refreshKeyToPaZoneGroupRecordMap(ulSchematicLocKey);
            refreshKeyToPaZoneRecordMap(ulSchematicLocKey);
        }

        // Only want to load up our local zones/DVA Version data if at a station
        if ( CachedConfig::getInstance()->isAtOcc() )
        {
            refreshKeyToDvaVersionRecordMap();
        }
        else
        {
            refreshKeyToDvaVersionRecordMap(ulLocKey);
        }

        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaStationRecordMap();

		// load TisMessage
        TisCachedMaps::getInstance().refreshPaTisMapping( ulLocKey, CachedConfig::getInstance()->isSchematicAtOcc() );

        // Tell the online update listener we want to automatically register
        // any new keys as they come through
        TA_Base_Core::OnlineUpdateListener::getInstance().setListenOutForNewKeys(true);

        // Begin listening for out for updates for subset of maps
        // PA Zone and PA Zone Group updates handled
        TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().startListening();
        TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().startListening();

        // Pa Station DVA Message updates listened out for
        TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap().startListening();

        //PA DVA Version updates listened
        TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap().startListening();
    }

    void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey)
    {
        FUNCTION_ENTRY("refreshKeyToPaZoneGroupRecordMap");

        // Zone groups are not loaded by PA Agent, must load directly from DB
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneGroupRecordMap(locationKey);

        FUNCTION_EXIT;
    }

    void refreshKeyToPaZoneRecordMap(unsigned long locationKey)
    {
        FUNCTION_ENTRY("refreshKeyToPaZoneRecordMap");

        // Static data, load directly from DB
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneRecordMap(locationKey);

        FUNCTION_EXIT;
    }

    void refreshKeyToDvaVersionRecordMap(unsigned long locationKey)
    {
        FUNCTION_ENTRY("refreshKeyToDvaVersionRecordMap");

        // Want to load all data for these items, regardless of location        
		try
		{
			// Attempt to retrieve from agent
            TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* ret = NULL;
            CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), getDvaVersionRecords, () );
            std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords> messages( ret );

			PaDvaVersionCachedMap& cachedMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap();

			// Clear all entries before refreshing items
			cachedMap.clearKeyToRecordMap();

			for (unsigned int i = 0; i < messages->length(); i ++)
			{
				const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecord& msg = (*messages)[i];
				
                if (( msg.ulLocationKey != locationKey ) && ( -1 != locationKey ))
                {
                    continue;
                }

				DvaVersionRecord record;

				record.ulKey           = msg.ulKey;
				record.ulLocationKey    = msg.ulLocationKey;
				record.strVersionNumber = msg.strVersionNumber;
				
				// Insert the items directly into cached map
				cachedMap.updateRecordMapWithItem(record.ulKey, record);
			}
			
			// Must tell the cached map it's been initialised, else it will assert
			cachedMap.flagAsInitialised();
		}
		catch (...)
		{
			// Fall back to direct DB access
			TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaVersionRecordMap(locationKey);
		}
		
        FUNCTION_EXIT;
    }

    void refreshKeyToStationDvaMessageRecordMap(unsigned int locationKey)
    {   
        FUNCTION_ENTRY("refreshKeyToStationDvaMessageRecordMap");

        try
        {
            // Get all records local to the PA Agent, store those with matching location key
            // in our cache
            TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData* ret = NULL;
            CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), getStationDvaMessageRecords, () );
            std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData> messages( ret );

            // Clear all entries before refreshing items
            PaStationDvaMessageCachedMap& cachedMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap();
            
            cachedMap.clearKeyToRecordMap();

            for (unsigned int i = 0; i < messages->length(); i ++)
            {
                const TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecord& msg = (*messages)[i];

                if (-1 != locationKey)
                {
                    // If a location key is set, reject any records returned by Agent with mismatched location key
                    if (locationKey != msg.m_locationKey)
                    {
                        continue;
                    }
                }
                
                TA_IRS_Bus::StationDvaMessageRecord record;

		        record.ulKey = msg.m_pKey;
		        record.ulLocationKey = msg.m_locationKey;
		        record.usMsgId = static_cast<unsigned short>(msg.m_id);
		        record.strMsgName = msg.m_label.in();
		        record.usMsgType = msg.m_type;
                
                // Insert the items directly into cached map
                cachedMap.updateRecordMapWithItem(record.ulKey, record);
            }

            // Must tell the cached map it's been initialised, else it will assert
            cachedMap.flagAsInitialised();
        }
        catch (...)
        {
            // If there is any kind of failure, simply fall back to direct DB access
            TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToStationDvaMessageRecordMap(locationKey);
        }

        FUNCTION_EXIT;
    }

}   // namespace CachedMapLoader
