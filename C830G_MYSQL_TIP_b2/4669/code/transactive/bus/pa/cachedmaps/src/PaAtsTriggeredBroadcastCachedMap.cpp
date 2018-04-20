/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2009/11/20 09:58:14 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of PA ATS Triggered Broadcast mappings, also listens
  * out for changes to database regarding the PA_ATS_TRIGGERED_BROADCAST
  * table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>

#include "bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

namespace TA_IRS_Bus
{
    PaAtsTriggeredBroadcastCachedMap::PaAtsTriggeredBroadcastCachedMap()
    :
    GenericCachedMap<PaAtsTriggeredBroadcastRecord, TA_Base_Core::IPaAtsTriggeredBroadcast>
        (TA_Base_Core::PA_ATS_TRIGGERED_BROADCAST, "PaAtsTriggeredBroadcast")
    {
    }


    TA_Base_Core::IPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::getInstance().getPaAtsTriggeredBroadcast(pKey, readWrite);
    }


    void PaAtsTriggeredBroadcastCachedMap::validateRecordData(const PaAtsTriggeredBroadcastRecord& bcast)
    {
        // Validate various parameters (failure on a true condition met)
        // Event type can be zero..
        //performDatavalueValidation(0 == bcast.m_eventType, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_locationKey, bcast.m_pKey);
        
		// Bohong commended this for CL-20888
		// performDatavalueValidation(0 == bcast.m_destinationLocationKey, bcast.m_pKey);

        performDatavalueValidation(0 == bcast.m_platformId, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_paZoneGroupKey, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_paDvaMessageKey, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_dvaMessageVersion.length(), bcast.m_pKey);
    }

         
    PaAtsTriggeredBroadcastRecord PaAtsTriggeredBroadcastCachedMap::createRecordFromInterface(TA_Base_Core::IPaAtsTriggeredBroadcast& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        PaAtsTriggeredBroadcastRecord result;

        result.m_pKey = item.getKey();
        result.m_eventType = item.getAtsEventType();
        result.m_locationKey = item.getLocationKey();
        result.m_destinationLocationKey = item.getDestinationLocationKey();
        result.m_platformId = item.getPlatformId();
        result.m_isLastTrain = item.getIsLastTrain();
        result.m_paZoneGroupKey = item.getPaZoneGroupKey();
        result.m_paDvaMessageKey = item.getPaDvaMessageKey();
        result.m_isEnabled = item.getIsEnabledParameter();
        result.m_dvaMessageVersion = item.getDvaMessageVersion();

        return result;
    }


    void PaAtsTriggeredBroadcastCachedMap::refreshKeyToPaAtsTriggeredBroadcastRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToPaAtsTriggeredBroadcastRecordMap");

        TA_Base_Core::IPaAtsTriggeredBroadcasts broadcasts;

        if (-1 == locationKey)
        {
            // Obtain read only array of *all* ATS triggered broadcast messages
            broadcasts = TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::getInstance().getAllPaAtsTriggeredBroadcasts(false);
        }
        else
        {
            // Obtain read only array of location specific ATS triggered broadcast messages
            broadcasts = TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::getInstance().getPaAtsTriggeredBroadcastsByLocationKey(locationKey, false);
        }    

        try
        {
            refreshKeyToRecordMapUsing(broadcasts);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(broadcasts);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(broadcasts);

        FUNCTION_EXIT;
    }

    PaAtsTriggeredBroadcastRecord PaAtsTriggeredBroadcastCachedMap::getAtsTriggeredBroadcastRecordFromDetail(
            unsigned long eventType, 
            unsigned long locationKey, 
            unsigned long destinationLocationKey,
            unsigned long platformId,
            bool          isLastTrain) 
    {
        FUNCTION_ENTRY("getAtsTriggeredBroadcastRecordFromDetail");

        FilterATSRecordByDetail filter(eventType,
                                        locationKey,
                                        destinationLocationKey,
                                        platformId,
                                        isLastTrain);

        PaAtsTriggeredBroadcastRecords matchingBroadcasts = getFirstMatchingRecord(filter);

        if (matchingBroadcasts.size() > 0)
        {
            return matchingBroadcasts.front();
        }
        else
        {
            PaAtsTriggeredBroadcastRecord emptyRecord;
            emptyRecord.m_isEnabled = false;
            return emptyRecord;
        }
    }


    PaAtsTriggeredBroadcastCachedMap::FilterATSRecordByDetail::FilterATSRecordByDetail
    (
     unsigned long eventType, 
     unsigned long locationKey, 
     unsigned long destinationLocationKey,
     unsigned long platformId,
     bool          isLastTrain
    )
    :
     m_eventType(eventType), 
     m_locationKey(locationKey), 
     m_destinationLocationKey(destinationLocationKey),
     m_platformId(platformId),
     m_isLastTrain(isLastTrain)
    {
    }


    bool PaAtsTriggeredBroadcastCachedMap::FilterATSRecordByDetail::isMatch(
        const PaAtsTriggeredBroadcastRecord& record) const
    {
        if ( record.m_eventType              == m_eventType &&
             record.m_locationKey            == m_locationKey &&
             record.m_destinationLocationKey == m_destinationLocationKey &&
             record.m_platformId             == m_platformId &&
             record.m_isLastTrain            == m_isLastTrain )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

} // end namespace TA_IRS_Bus