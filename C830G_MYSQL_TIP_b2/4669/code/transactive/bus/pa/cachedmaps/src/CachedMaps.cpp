/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/CachedMaps.cpp $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Maintains all the cached maps for PA applications
  *
  */
#ifndef   _WIN32_WINNT   
  #define   _WIN32_WINNT   0x400   
  #endif

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include <set>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/ICachedMapObserver.h"

#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"

#include "bus/pa/cachedmaps/src/PaDvaPublicVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaPrivateVersionCachedMap.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"


using namespace TA_Base_Core;
//using namespace TA_IRS_Bus;


///////////// Our list of debug DVA messages
#include "bus/pa/cachedmaps/src/FakeMessagesArray.h"

namespace TA_IRS_Bus
{
    const std::string CachedMaps::PA_ZONE_STATUS_TOKEN     = "diiPAC-Status";
    const std::string CachedMaps::PA_ZONE_PRIORITY_TOKEN   = "aiiPAC-Priority";
    const std::string CachedMaps::PA_ZONE_LIVE_TOKEN       = "diiPAC-Live";
    const std::string CachedMaps::PA_ZONE_CYCLIC_TOKEN     = "diiPAC-Cyclic";
    const std::string CachedMaps::PA_ZONE_MUSIC_TOKEN      = "diiPAC-Music";

    CachedMaps* CachedMaps::m_me = NULL;
    CachedMaps::LocationNameHelper CachedMaps::s_locationNameHelper;

    NonReEntrantThreadLockable CachedMaps::m_lockForSingleton;

    static void toUpper(std::string& convertMe)
    {
        std::transform(convertMe.begin(), convertMe.end(), convertMe.begin(), toupper);
    }


    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    CachedMaps::CachedMaps()
     : 
       m_isLocationKeyToNameMapInitialised(false),
       m_isZoneIdToDataPointMapsInitialised(false),
       m_paZoneCachedMap(new PaZoneCachedMap(s_locationNameHelper)),
       m_paZoneGroupCachedMap(new PaZoneGroupCachedMap()),
       m_paStationDvaMessageCachedMap(new PaStationDvaMessageCachedMap()),
       m_paTrainDvaMessageCachedMap(new PaTrainDvaMessageCachedMap()),
       m_paDvaVersionCachedMap(new PaDvaVersionCachedMap()),
       m_paAtsTriggeredBroadcastCachedMap(new PaAtsTriggeredBroadcastCachedMap()),
       m_paStationCachedMap(new PaStationCachedMap()),
       m_paDvaPublicVersionCachedMap(new PaDvaPublicVersionCachedMap()),
       m_paDvaPrivateVersionCachedMap(new PaDvaPrivateVersionCachedMap())
    {
        m_zoneIdToStatusDataPointMap.clear();
        m_zoneIdToPriorityDataPointMap.clear();
        m_zoneIdToLiveDataPointMap.clear();
        m_zoneIdToCyclicDataPointMap.clear();
        m_zoneIdToMusicDataPointMap.clear();
    }


    CachedMaps::~CachedMaps()
    {
        // Delete all existing heap objects
        try
        {
            delete m_paZoneCachedMap;
            m_paZoneCachedMap = 0;
    
            delete m_paZoneGroupCachedMap;
            m_paZoneGroupCachedMap = 0;
    
            delete m_paStationDvaMessageCachedMap;
            m_paStationDvaMessageCachedMap = 0;

            delete m_paTrainDvaMessageCachedMap;
            m_paTrainDvaMessageCachedMap = 0;

            delete m_paDvaVersionCachedMap;
            m_paDvaVersionCachedMap = 0;

            delete m_paAtsTriggeredBroadcastCachedMap;
            m_paAtsTriggeredBroadcastCachedMap = 0;

            delete m_paStationCachedMap;
            m_paStationCachedMap = 0;

            delete m_paDvaPublicVersionCachedMap;
            m_paDvaPublicVersionCachedMap = 0;

            delete m_paDvaPrivateVersionCachedMap;
            m_paDvaPrivateVersionCachedMap = 0;
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    CachedMaps* CachedMaps::getInstance()
    {
	    if ( 0 == m_me )
	    {
		    // Double checking to prevent multiple threads
		    // creating multiple instances.

            // Any lock is okay at this stage.
		    ThreadGuard guard( m_lockForSingleton );

		    if ( 0 == m_me )
		    {
			    m_me = new CachedMaps();
    	    }
	    }

	    return m_me;
    }

    void CachedMaps::removeInstance( )
    {
	    // 
	    // Guard this to prevent multiple threads atempting
	    // to delete/create simultaneously
	    //
	    ThreadGuard guard( m_lockForSingleton );
	    if ( m_me != NULL )
	    {
		    delete m_me;
		    m_me = NULL;
	    }
    }

    void CachedMaps::validateAllInitialisedMaps() //throw(TA_Base_Core::InvalidPasConfigurationException)
    {
        // These will only be executed if the associated maps have been initialised.
    }





    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Train DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToTrainDvaMessageRecordMap() 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paTrainDvaMessageCachedMap->refreshKeyToTrainDvaMessageRecordMap();
    }


    TrainDvaMessageRecord CachedMaps::getTrainDvaMessageRecordFromKey(unsigned long key)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paTrainDvaMessageCachedMap->getCachedDbRecordFromKey(key);
    }


    TrainDvaMessageRecords CachedMaps::getTrainDvaMessageRecords(const std::string& filterKeyword)
    {
        return m_paTrainDvaMessageCachedMap->getTrainDvaMessageRecords(filterKeyword);
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////

    unsigned long CachedMaps::getKeyFromStationDvaMessageIdAndLocation(unsigned long messageId, unsigned long locationKey)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->getKeyFromStationDvaMessageIdAndLocation(messageId, locationKey);
    }


    StationDvaMessageRecord CachedMaps::getStationDvaMessageRecordFromKey(unsigned long key)
       //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->getCachedDbRecordFromKey(key);
    }


    unsigned long CachedMaps::getStationDvaMessageLocationKeyFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return getStationDvaMessageRecordFromKey(key).m_locationKey;
    }


    StationDvaMessageRecordPointers CachedMaps::getClonedAdhocMessageRecords
            ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet )
    {
        return m_paStationDvaMessageCachedMap->getClonedAdhocMessageRecords(dvaSet);
    }


    unsigned long CachedMaps::getStationDvaMessageIdFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return getStationDvaMessageRecordFromKey(key).m_id;
    }


    bool CachedMaps::processDvaMessageUpdates(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->processDvaMessageUpdates(updateData);
    }


    void CachedMaps::setStationDvaMessageRecord(StationDvaMessageRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paStationDvaMessageCachedMap->setStationDvaMessageRecord(record, writeChangesToDatabase);
    }


    StationDvaMessageRecordPointers CachedMaps::getClonedPrerecordedMessageRecords
        ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet, 
          char messageType,
          const std::string& filterKeyword)
    {
        return m_paStationDvaMessageCachedMap->getClonedPrerecordedMessageRecords(
                                                                    dvaSet,
                                                                    messageType,
                                                                    filterKeyword);
    }


    void CachedMaps::refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paStationDvaMessageCachedMap->refreshKeyToStationDvaMessageRecordMap(locationKey);
    }
    

    void CachedMaps::registerPaStationDvaMessageChangeObserver(ICachedMapObserver* observer)
    {
        m_paStationDvaMessageCachedMap->registerObserver(observer);
    }


    void CachedMaps::deRegisterPaStationDvaMessageChangeObserver(ICachedMapObserver* observer)
    {
        m_paStationDvaMessageCachedMap->deRegisterObserver(observer);
    }


    TA_Base_Bus::IPAAgentCorbaDef::EDvaSet CachedMaps::getStationDvaMessageSetFromKey(unsigned long messageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        StationDvaMessageRecord record = getStationDvaMessageRecordFromKey(messageKey);

        if (record.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
        {
            // 1 - 500
            if (record.m_locationKey == LocationAccessFactory::getInstance().getOccLocationKey())
            {
                // OCC Set A
                return TA_Base_Bus::IPAAgentCorbaDef::SetA;
            }
            else
            {
                // Depot Set D
                return TA_Base_Bus::IPAAgentCorbaDef::SetD;
            }
        }
        else if (record.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
                                TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
        {
            // 501-1000

            // Station Set B
            return TA_Base_Bus::IPAAgentCorbaDef::SetB;
        }
    
        return TA_Base_Bus::IPAAgentCorbaDef::SetUnknown;
    }

    bool CachedMaps::isPreRecordedMessage(unsigned long messageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        unsigned long messageId = getStationDvaMessageIdFromKey(messageKey);

        if (messageId >= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1 &&
            messageId <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
        {
            // Set A / Set D Pre-Recorded
            return true;
        }

        if (messageId >= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1 
                         &&
            messageId <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
        {
            // Set B Pre-Recorded
            return true;
        }

        return false;
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToDvaVersionRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paDvaVersionCachedMap->refreshKeyToRecordMap(locationKey);
    }

    DvaVersionRecord CachedMaps::getDvaVersionRecordFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaVersionCachedMap->getRecordFromLocation(locationKey);
    }

    unsigned long CachedMaps::getKeyFromDvaVersionRecordLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaVersionCachedMap->getKeyFromLocation(locationKey);
    }


    std::string CachedMaps::getDvaMessageVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        StationDvaMessageRecord messageRecord = 
            getStationDvaMessageRecordFromKey(stationDvaMessageKey);

        DvaVersionRecord versionRecord = getDvaVersionRecordFromLocationKey(messageRecord.m_locationKey);
        return getDvaMessageVersionFromStationDvaMessage(messageRecord, versionRecord);
    }


    std::string CachedMaps::getDvaMessageVersionFromStationDvaMessage(const StationDvaMessageRecord& messageRecord,
                            const DvaVersionRecord& versionRecord)
    {
        TA_ASSERT(versionRecord.m_locationKey == messageRecord.m_locationKey, "Mismatched location keys");

        if (messageRecord.m_locationKey == LocationAccessFactory::getInstance().getOccLocationKey())
        {
            // OCC Set A
            switch (messageRecord.m_id)
            {
            case 1:
                return versionRecord.m_publicAdhoc1;
            case 2:
                return versionRecord.m_publicAdhoc2;
            case 3:
                return versionRecord.m_publicAdhoc3;
            case 4:
                return versionRecord.m_publicAdhoc4;
            case 5:
                return versionRecord.m_publicAdhoc5;
            case 6:
                return versionRecord.m_publicAdhoc6;

            default:
                // >6 and <=500
                if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC &&
                    messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
                {
                    return versionRecord.m_publicPreRecorded;
                }
                else 
                {
                    std::ostringstream msg;
                    msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
                        << " in KeyToPaDvaVersionRecordMap";
		            TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
                }
                // ToDo - validate by checking the range of messageIds
            }
        }
        else if (messageRecord.m_locationKey == LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Depot Set D
            switch (messageRecord.m_id)
            {
            case 1:
                return versionRecord.m_privateAdhoc1;
            case 2:
                return versionRecord.m_privateAdhoc2;
            case 3:
                return versionRecord.m_privateAdhoc3;
            case 4:
                return versionRecord.m_privateAdhoc4;
            case 5:
                return versionRecord.m_privateAdhoc5;
            case 6:
                return versionRecord.m_privateAdhoc6;

            default:
                // >6 and <=500
                if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC &&
                    messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
                {
                    return versionRecord.m_privatePreRecorded;
                }
                else 
                {
                    std::ostringstream msg;
                    msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
                        << " in KeyToPaDvaVersionRecordMap";
		            TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
                }
            }
        }
        else
        {
            switch (messageRecord.m_id)
            {
            case 1:
                return versionRecord.m_publicAdhoc1;
            case 2:
                return versionRecord.m_publicAdhoc2;
            case 3:
                return versionRecord.m_publicAdhoc3;
            case 4:
                return versionRecord.m_publicAdhoc4;
            case 5:
                return versionRecord.m_publicAdhoc5;
            case 6:
                return versionRecord.m_publicAdhoc6;

            case 501:
                return versionRecord.m_privateAdhoc1;
            case 502:
                return versionRecord.m_privateAdhoc2;
            case 503:
                return versionRecord.m_privateAdhoc3;
            case 504:
                return versionRecord.m_privateAdhoc4;
            case 505:
                return versionRecord.m_privateAdhoc5;
            case 506:
                return versionRecord.m_privateAdhoc6;

            default:
            
                if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC &&
                    messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
                {
                    // >6 and <=500
                    return versionRecord.m_publicPreRecorded;
                }
                else
                if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC &&
                    messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                                          TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC )
                {
                    // >506 <=1000
                    return versionRecord.m_privatePreRecorded;
                }   
                else
                {
                    std::ostringstream msg;
                    msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
                        << " in KeyToPaDvaVersionRecordMap";
		            TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
                }
            }
        }

        return "";
    }

    void CachedMaps::setDvaMessageVersionRecord(DvaVersionRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paDvaVersionCachedMap->setRecord(record, writeChangesToDatabase);
    }

	
    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Public Message Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToDvaPublicVersionRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paDvaPublicVersionCachedMap->refreshKeyToRecordMap(locationKey);
    }

    DvaPublicVersionRecord CachedMaps::getDvaPublicVersionRecordFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaPublicVersionCachedMap->getRecordFromLocation(locationKey);
    }

    unsigned long CachedMaps::getKeyFromDvaPublicVersionRecordLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaPublicVersionCachedMap->getKeyFromLocation(locationKey);
    }


    std::string CachedMaps::getDvaMessagePublicVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        StationDvaMessageRecord messageRecord = 
            getStationDvaMessageRecordFromKey(stationDvaMessageKey);

        DvaPublicVersionRecord versionRecord = getDvaPublicVersionRecordFromLocationKey(messageRecord.m_locationKey);
        return getDvaMessagePublicVersionFromStationDvaMessage(messageRecord, versionRecord);
    }


    std::string CachedMaps::getDvaMessagePublicVersionFromStationDvaMessage(const StationDvaMessageRecord& messageRecord,
                            const DvaPublicVersionRecord& versionRecord)
    {
        TA_ASSERT(versionRecord.m_locationKey == messageRecord.m_locationKey, "Mismatched location keys");
		
		switch (messageRecord.m_id)
		{
		case 1:
			return versionRecord.m_publicAdhoc1;
		case 2:
			return versionRecord.m_publicAdhoc2;
		case 3:
			return versionRecord.m_publicAdhoc3;
		case 4:
			return versionRecord.m_publicAdhoc4;
		case 5:
			return versionRecord.m_publicAdhoc5;
		case 6:
			return versionRecord.m_publicAdhoc6;
			
		default:
			// >6 and <=500
			if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC &&
				messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
			{
				return versionRecord.m_publicPreRecorded;
			}
			else 
			{
				std::ostringstream msg;
				msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
					<< " in KeyToPaDvaVersionRecordMap";
				TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
			}
			// ToDo - validate by checking the range of messageIds
		}

		return "";
    }

    void CachedMaps::setDvaMessagePublicVersionRecord(DvaPublicVersionRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paDvaPublicVersionCachedMap->setRecord(record, writeChangesToDatabase);
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Private Message Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToDvaPrivateVersionRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paDvaPrivateVersionCachedMap->refreshKeyToRecordMap(locationKey);
    }

    DvaPrivateVersionRecord CachedMaps::getDvaPrivateVersionRecordFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaPrivateVersionCachedMap->getRecordFromLocation(locationKey);
    }

    unsigned long CachedMaps::getKeyFromDvaPrivateVersionRecordLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaPrivateVersionCachedMap->getKeyFromLocation(locationKey);
    }


    std::string CachedMaps::getDvaMessagePrivateVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        StationDvaMessageRecord messageRecord = 
            getStationDvaMessageRecordFromKey(stationDvaMessageKey);

        DvaPrivateVersionRecord versionRecord = getDvaPrivateVersionRecordFromLocationKey(messageRecord.m_locationKey);
        return getDvaMessagePrivateVersionFromStationDvaMessage(messageRecord, versionRecord);
    }


    std::string CachedMaps::getDvaMessagePrivateVersionFromStationDvaMessage(const StationDvaMessageRecord& messageRecord,
                            const DvaPrivateVersionRecord& versionRecord)
    {
        TA_ASSERT(versionRecord.m_locationKey == messageRecord.m_locationKey, "Mismatched location keys");
		
        // TD17634
		if (messageRecord.m_locationKey == LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Depot Set D
            switch (messageRecord.m_id)
            {
            case 1:
                return versionRecord.m_privateAdhoc1;
            case 2:
                return versionRecord.m_privateAdhoc2;
            case 3:
                return versionRecord.m_privateAdhoc3;
            case 4:
                return versionRecord.m_privateAdhoc4;
            case 5:
                return versionRecord.m_privateAdhoc5;
            case 6:
                return versionRecord.m_privateAdhoc6;

            default:
                // >6 and <=500
                if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC &&
                    messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC)
                {
                    return versionRecord.m_privatePreRecorded;
                }
                else 
                {
                    std::ostringstream msg;
                    msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
                        << " in KeyToPaDvaVersionRecordMap";
		            TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
                }
            }
        }
		else
		{
			// set B for station
			switch (messageRecord.m_id)
			{
			case 501:
				return versionRecord.m_privateAdhoc1;
			case 502:
				return versionRecord.m_privateAdhoc2;
			case 503:
				return versionRecord.m_privateAdhoc3;
			case 504:
				return versionRecord.m_privateAdhoc4;
			case 505:
				return versionRecord.m_privateAdhoc5;
			case 506:
				return versionRecord.m_privateAdhoc6;
				
			default:
				// >506 and <=1000
				if (messageRecord.m_id >  TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
					TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
					TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC &&
					messageRecord.m_id <= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
					TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
					TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
					TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC )
				{
					return versionRecord.m_privatePreRecorded;
				}
				else 
				{
					std::ostringstream msg;
					msg << "Failed to locate dva message version for station dva message key =" << messageRecord.m_pKey
						<< " in KeyToPaDvaVersionRecordMap";
					TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
				}
			}
		}
        // TD17634

        return "";
    }

    void CachedMaps::setDvaMessagePrivateVersionRecord(DvaPrivateVersionRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paDvaPrivateVersionCachedMap->setRecord(record, writeChangesToDatabase);
    }


    //////////////////////////////////////////////////////////////////////
    //
    // LocationKeyToNameMap methods
    //
    //////////////////////////////////////////////////////////////////////
    void CachedMaps::refreshLocationKeyToNameMap() 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        // Get all location records. 

        std::vector<ILocation*> allLocations = 
            LocationAccessFactory::getInstance().getAllLocations(false);

        ThreadGuard guard( m_lockForLocationKeyToNameMap );
        m_locationKeyToNameMap.clear();

        std::vector<ILocation*>::iterator it = allLocations.begin();
        for (;it!= allLocations.end(); ++it)
        {
            unsigned long locationKey  = (*it)->getKey();
            std::string   locationName = (*it)->getName();
        
            // No point checking for one-to-one relationship as the database constraints 
            // should prevent this.

            m_locationKeyToNameMap[locationKey] = locationName;

            // Prevent memory leak
            delete (*it);
        }

        allLocations.clear();

        m_isLocationKeyToNameMapInitialised = true;
    }


    std::string CachedMaps::getLocationNameFromKey(unsigned long locationKey) 
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {   
        // No explicit call to refreshLocationKeyToNameMap() is necessary by the client
        // We will refresh the map if required.
        if (!m_isLocationKeyToNameMapInitialised)
        {
            refreshLocationKeyToNameMap();
        }

        ThreadGuard guard( m_lockForLocationKeyToNameMap);

        LocationKeyToNameMapIt it = m_locationKeyToNameMap.find(locationKey);
	    if (it == m_locationKeyToNameMap.end())
	    {
            std::ostringstream msg;
            msg << "Failed to locate locationName for locationKey=" << locationKey
                << " in LocationKeyToNameMap";
		    TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
	    }

	    return it->second;
    }


    unsigned long CachedMaps::getLocationKeyFromName(const std::string& locationName) 
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        // No explicit call to refreshLocationKeyToNameMap() is necessary by the client
        // We will refresh the map if required.   
        if (!m_isLocationKeyToNameMapInitialised)
        {
            refreshLocationKeyToNameMap();
        }


        ThreadGuard guard( m_lockForLocationKeyToNameMap );

        LocationKeyToNameMapIt it = m_locationKeyToNameMap.begin();
        for ( ; it != m_locationKeyToNameMap.end(); ++it)
        {
            if (it->second == locationName)
            {
                return it->first;
            }
        }

        std::ostringstream msg;
        msg << "Failed to locate locationKey for locationName=" << locationName 
            << " in LocationKeyToNameMap";
	    TA_THROW(CachedMappingNotFoundException(msg.str().c_str()));
    }


    //////////////////////////////////////////////////////////////////////
    //
    // PasStationIdToLocationNameMap methods
    //
    //////////////////////////////////////////////////////////////////////
    void CachedMaps::refreshKeyToPaStationRecordMap(unsigned long locationKey)
    {
        m_paStationCachedMap->refreshKeyToPaStationRecordMap(locationKey);
    }


    unsigned int CachedMaps::getPasStationIdFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationCachedMap->getPasStationIdFromLocationKey(locationKey);
    }


    unsigned long CachedMaps::getLocationKeyFromPasStationId(unsigned int stationId)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationCachedMap->getLocationKeyFromPasStationId(stationId);
    }


    //////////////////////////////////////////////////////////////////////
    //
    // ZoneIdToDataPointMaps methods
    //
    //////////////////////////////////////////////////////////////////////


    void CachedMaps::refreshZoneIdToDataPointMap(const std::vector<TA_Base_Bus::DataPoint*>& pasDataPoints) 
        //throw(TA_Base_Core::InvalidPasConfigurationException)
    {
        ThreadGuard guard( m_lockForZoneIdToDataPointMaps );

        m_zoneIdToStatusDataPointMap.clear();
        m_zoneIdToPriorityDataPointMap.clear();
        m_zoneIdToCyclicDataPointMap.clear();
        m_zoneIdToLiveDataPointMap.clear();
        m_zoneIdToMusicDataPointMap.clear();

        unsigned long occLocationKey = LocationAccessFactory::getInstance().getOccLocationKey();

        std::vector<TA_Base_Bus::DataPoint*>::const_iterator it;
        for (it = pasDataPoints.begin(); it!=pasDataPoints.end(); ++it)
        {
            TA_Base_Bus::DataPoint* dp = *it;
        
            // Determine the target map
            std::string dpName = dp->getDataPointName();
            ZoneIdToDataPointMap* theMap = NULL;

            if (dpName.find(PA_ZONE_STATUS_TOKEN) != std::string::npos)
            {
                theMap = &m_zoneIdToStatusDataPointMap;
            }
            else if (dpName.find(PA_ZONE_PRIORITY_TOKEN) != std::string::npos)
            {
                theMap = &m_zoneIdToPriorityDataPointMap;
            }
            else if (dpName.find(PA_ZONE_CYCLIC_TOKEN) != std::string::npos)
            {
                theMap = &m_zoneIdToCyclicDataPointMap;
            }
            else if (dpName.find(PA_ZONE_LIVE_TOKEN) != std::string::npos)
            {
                theMap = &m_zoneIdToLiveDataPointMap;
            }
            else if (dpName.find(PA_ZONE_MUSIC_TOKEN) != std::string::npos)
            {
                theMap = &m_zoneIdToMusicDataPointMap;
            }
            else
            {
                // Ignore
                continue;
            }

            std::string::size_type lastDotOffset = dpName.find_last_of('.');
            if (lastDotOffset == std::string::npos)
            {
                // Ignore
                continue;            
            }

            
            std::string equipmentName = dpName.substr(0, lastDotOffset);
            
            unsigned long zoneId(0);

            try
            {
                PaZoneRecord record = getRecordFromEquipmentEntityName(equipmentName);

                if (dp->getLocationKey() == occLocationKey)
                {
                    // This is a OCC copy of the station datapoint.  So use absolute addressing
                    m_paStationCachedMap->relativeToAbsoluteZoneId(zoneId, record.m_id, record.m_locationKey);
                }
                else
                {
                    zoneId = record.m_id;
                }
            }
            catch(TA_Base_Core::CachedMappingNotFoundException&)
            {
                // Ignore - some OCC datapoints may not have been 
                // tied to PA Zones yet.
                continue;
            }
            

            // Check if mapping already exists.
            if (theMap->find(zoneId) != theMap->end())
            {
                // Map has already been added, relationship is not one-to-one.

                std::ostringstream msg;
                msg << "Inconsistent PAS configuration detected. "
                    << "ZoneId=" <<  zoneId 
                    << " is mapped to more than one datapoint.";  
                TA_THROW( InvalidPasConfigurationException(msg.str().c_str()) );
            }

            (*theMap)[zoneId] = dp;
        }

        m_isZoneIdToDataPointMapsInitialised = true;
    }

    TA_Base_Bus::DataPoint* CachedMaps::getDataPointFromZoneId(unsigned long zoneId, ZoneIdToDataPointMap& theMap)
    {
        TA_ASSERT(m_isZoneIdToDataPointMapsInitialised, "ZoneIdToDataPointMaps not initialised");

        ThreadGuard guard( m_lockForZoneIdToDataPointMaps );

        ZoneIdToDataPointMapIt it = theMap.find(zoneId);

	    if (it == theMap.end())
	    {
            return NULL;
	    }

	    return it->second;
    }


    void CachedMaps::setOccDepotFlags(bool isOcc, bool isDepot)
    {
        TA_ASSERT(!(isOcc && isDepot), "Cannot have both parameters as true");

        if (isOcc)
        {
            m_locationType.setValue(TA_Base_Core::IPaDvaMessageVersion::OCC);
        }
        else if (isDepot)
        {
            m_locationType.setValue(TA_Base_Core::IPaDvaMessageVersion::DEPOT);
        }
        else if (!isOcc && !isDepot)
        {
            m_locationType.setValue(TA_Base_Core::IPaDvaMessageVersion::STATION);
        }
    }


    TA_Base_Core::IPaDvaMessageVersion::ELocation CachedMaps::getLocationType()
    {
        TA_ASSERT(m_locationType.hasBeenSet(), "locationType has not been set");

        return m_locationType.getValue();
    }

} // end namespace TA_IRS_Bus