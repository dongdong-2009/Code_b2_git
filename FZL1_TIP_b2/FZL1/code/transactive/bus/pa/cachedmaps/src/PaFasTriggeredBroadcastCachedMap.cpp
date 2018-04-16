/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * Maintains a cache of PA ATS Triggered Broadcast mappings, also listens
  * out for changes to database regarding the PA_FAS_TRIGGERED_BROADCAST
  * table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>

#include "bus/pa/cachedmaps/src/PaFasTriggeredBroadcastCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

namespace TA_IRS_Bus
{
    PaFasTriggeredBroadcastCachedMap::PaFasTriggeredBroadcastCachedMap()
    :
    GenericCachedMap<PaFasTriggeredBroadcastRecord, TA_Base_Core::IPaFasTriggeredBroadcast>
        (TA_Base_Core::PA_FAS_TRIGGERED_BROADCAST, "PaFasTriggeredBroadcast")
    {
    }


    TA_Base_Core::IPaFasTriggeredBroadcast* PaFasTriggeredBroadcastCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::getInstance().getPaFasTriggeredBroadcast(pKey, readWrite);
    }


    void PaFasTriggeredBroadcastCachedMap::validateRecordData(const PaFasTriggeredBroadcastRecord& bcast)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == bcast.m_ulAlarmEntityKey, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_locationKey, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_paZoneGroupKey, bcast.m_pKey);
        performDatavalueValidation(0 == bcast.m_paDvaMessageKey, bcast.m_pKey);
    }

         
    PaFasTriggeredBroadcastRecord PaFasTriggeredBroadcastCachedMap::createRecordFromInterface(TA_Base_Core::IPaFasTriggeredBroadcast& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        PaFasTriggeredBroadcastRecord result;

        result.m_pKey = item.getKey();
        result.m_ulAlarmEntityKey = item.getFasAlarmEntityKey();
        result.m_locationKey = item.getLocationKey();
        result.m_paDvaMessageKey = item.getPaDvaMessageKey();
        result.m_paCyclicTimes = item.getPaCyclicTimes();
        result.m_paZoneGroupKey = item.getPaZoneGroupKey();
        result.m_isEnabled = item.getIsEnabledParameter();

        return result;
    }


    void PaFasTriggeredBroadcastCachedMap::refreshKeyToPaFasTriggeredBroadcastRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToPaFasTriggeredBroadcastRecordMap");

        TA_Base_Core::IPaFasTriggeredBroadcasts broadcasts;

        if (-1 == locationKey)
        {
            // Obtain read only array of *all* ATS triggered broadcast messages
            broadcasts = TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::getInstance().getAllPaFasTriggeredBroadcasts(false);
        }
        else
        {
            // Obtain read only array of location specific ATS triggered broadcast messages
            broadcasts = TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::getInstance().getPaFasTriggeredBroadcastsByLocationKey(locationKey, false);
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PaFasTriggeredBroadcastCachedMap refreshed and loaded %d items of location %d", m_keyToRecordMap.size(), locationKey );

        FUNCTION_EXIT;
    }

    PaFasTriggeredBroadcastRecord PaFasTriggeredBroadcastCachedMap::getFasTriggeredBroadcastRecordFromDetail(
            unsigned long ulAlarmEntityKey ) 
    {
        FUNCTION_ENTRY("getFasTriggeredBroadcastRecordFromDetail");

        FilterATSRecordByDetail filter(ulAlarmEntityKey);

        PaFasTriggeredBroadcastRecords matchingBroadcasts = getFirstMatchingRecord(filter);

        if (matchingBroadcasts.size() > 0)
        {
            return matchingBroadcasts.front();
        }
        else
        {
            PaFasTriggeredBroadcastRecord emptyRecord;
            emptyRecord.m_isEnabled = false;
            return emptyRecord;
        }
    }


    PaFasTriggeredBroadcastCachedMap::FilterATSRecordByDetail::FilterATSRecordByDetail( unsigned long ulAlarmEntityKey ) :
    m_ulFasAlarmEntityKey(ulAlarmEntityKey)
    {
    }


    bool PaFasTriggeredBroadcastCachedMap::FilterATSRecordByDetail::isMatch(
        const PaFasTriggeredBroadcastRecord& record) const
    {
        if ( record.m_ulAlarmEntityKey == m_ulFasAlarmEntityKey  )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

} // end namespace TA_IRS_Bus