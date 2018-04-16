/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/CachedMapDefinitions.h $
  * @author Jade Lee
  * @version $Revision: #6 $
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by: $Author: qi.huang $
  * 
  * Provides some definitions used by the various cached maps 
  * (for use by client code)
  *
  */

#ifndef _CACHED_MAP_DEFINITIONS_HEADER_
#define _CACHED_MAP_DEFINITIONS_HEADER_

#include <list>
#include "core/data_access_interface/src/DatabaseKey.h"

namespace TA_IRS_Bus
{

    // Cannot forward declare when typedef in a list (done at end of file)
    //struct StationDvaMessageRecord;
    //struct PaZoneRecord;
    //struct PaZoneGroupRecord;

    struct TrainDvaMessageRecord
    {
        TrainDvaMessageRecord()
            : m_pKeyOrId(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_label("")
        {}
        // Default assignment and copy constructors should be ok
        unsigned long m_pKeyOrId;	    
	    std::string   m_label;

        bool operator==(const TrainDvaMessageRecord& s) const
        {
            return (m_pKeyOrId == s.m_pKeyOrId &&
                    m_label == s.m_label);
        }
    };   

    typedef std::list<TrainDvaMessageRecord>   TrainDvaMessageRecords;
    typedef TrainDvaMessageRecords::iterator   TrainDvaMessageRecordsIt;


    struct StationDvaMessageRecord
    {
        // Default assignment and copy constructors should be ok
        unsigned long ulKey;
        unsigned long ulLocationKey;
        unsigned short usMsgId;
        unsigned short usMsgType;
        std::string strMsgName;

        StationDvaMessageRecord() : 
        ulKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
        ulLocationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
        usMsgId(0),
        usMsgType(0),
        strMsgName("")
        {
        }

        bool operator==(const StationDvaMessageRecord& refCmp) const
        {
            return ( ulKey == refCmp.ulKey && ulLocationKey == refCmp.ulLocationKey &&
                usMsgId == refCmp.usMsgId && usMsgType == refCmp.usMsgType &&
                strMsgName == refCmp.strMsgName );
        }
    };

    typedef std::list<StationDvaMessageRecord>   StationDvaMessageRecords;
    typedef StationDvaMessageRecords::iterator   StationDvaMessageRecordsIt;

    struct PaZoneRecord
    {
        /// Defines where certain parameter extracted from entityname (from database)
        /// - used to define m_area and m_zoneShortName
        enum ETokenIdentifier
        {
            STATION = 0,
            PAMARK,
            AREA,
            ZONESHORTNAME
        };

        PaZoneRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_id(0),
              m_orderId(0u),
              m_strDispLocName(""),
              m_label(""),
              m_area(""),
              m_occZoneShortName(""),
              m_stnZoneShortName(""),
              m_strZoneOccLocName(""),
              m_strZoneStnLocName(""),
              m_stnEquipmentEntityName(""),
              m_occEquipmentEntityName("")
        {}
        // Default assignment and copy constructors should be ok
	    unsigned long           m_pKey;
        unsigned long           m_locationKey;
        std::string             m_strDispLocName;
        unsigned long           m_id;
        unsigned long           m_orderId;
	    std::string             m_label;
	    std::string             m_area;
        
        // We can derive this information from equipment entity name, 
		// stored purely for (filtering) efficiency reasons            
        std::string             m_occZoneShortName;
        std::string             m_stnZoneShortName;
        std::string             m_strZoneOccLocName;
        std::string             m_strZoneStnLocName;
        std::string             m_stnEquipmentEntityName;
        std::string             m_occEquipmentEntityName;

        bool operator==(const PaZoneRecord& s) const
        {
    	    return (m_pKey == s.m_pKey &&
                    m_locationKey  == s.m_locationKey &&
                    m_strDispLocName  == s.m_strDispLocName &&
                    m_strZoneOccLocName == s.m_strZoneOccLocName &&
                    m_strZoneStnLocName == s.m_strZoneStnLocName &&
                    m_id  == s.m_id &&
                    m_orderId == s.m_orderId &&
                    m_label  == s.m_label &&
                    m_area  == s.m_area &&
                    m_stnZoneShortName  == s.m_stnZoneShortName &&
                    m_occZoneShortName  == s.m_occZoneShortName &&
                    m_stnEquipmentEntityName == s.m_stnEquipmentEntityName &&
                    m_occEquipmentEntityName == s.m_occEquipmentEntityName);
        }
    };
    typedef std::list<PaZoneRecord>      PaZoneRecords;
    typedef PaZoneRecords::iterator      PaZoneRecordsIt;


    struct PaZoneGroupRecord
    {
        PaZoneGroupRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_label(""),
              m_isEventGroup(false)
        {}

        unsigned int m_pKey;
        unsigned int m_locationKey;
        std::string m_label;
        bool        m_isEventGroup;

        bool operator==(const PaZoneGroupRecord& s) const
        {
           return (m_pKey == s.m_pKey && 
                   m_locationKey == s.m_locationKey &&
                   m_label == s.m_label &&
                   m_isEventGroup == s.m_isEventGroup);
        }
    };

    typedef std::list<PaZoneGroupRecord>      PaZoneGroupRecords;
    typedef PaZoneGroupRecords::iterator      PaZoneGroupRecordsIt;


    struct DvaVersionRecord
    {
        DvaVersionRecord()
            : ulKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              ulLocationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              strVersionNumber("")
        {
        }

        // Default assignment and copy constructors should be ok
        unsigned long ulKey;
        unsigned long ulLocationKey;

        std::string   strVersionNumber;

        bool operator==(const DvaVersionRecord& s) const
        {
            return (ulKey == s.ulKey &&
                    ulLocationKey == s.ulLocationKey &&
            
                    strVersionNumber == s.strVersionNumber);        
        }
    };
    typedef std::list<DvaVersionRecord>      DvaVersionRecords;
    typedef DvaVersionRecords::iterator      DvaVersionRecordsIt;

    struct PaAtsTriggeredBroadcastRecord
    {
        PaAtsTriggeredBroadcastRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_eventType(0),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_destinationLocationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_platformId(0),
              m_isLastTrain(false),
              m_paZoneGroupKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_paDvaMessageKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_dvaMessageVersion(""),
              m_isEnabled(false)
        {}

        unsigned long m_pKey;
        unsigned long m_eventType;
        unsigned long m_locationKey;
        unsigned long m_destinationLocationKey;
        unsigned long m_platformId;
        bool          m_isLastTrain;
        unsigned long m_paZoneGroupKey;
        unsigned long m_paDvaMessageKey;
        std::string   m_dvaMessageVersion;
        bool          m_isEnabled;

        bool operator==(const PaAtsTriggeredBroadcastRecord& s) const
        {
            return (m_pKey== s.m_pKey &&
                    m_eventType== s.m_eventType &&
                    m_locationKey== s.m_locationKey &&
                    m_destinationLocationKey== s.m_destinationLocationKey &&
                    m_platformId== s.m_platformId &&
                    m_isLastTrain== s.m_isLastTrain &&
                    m_paZoneGroupKey== s.m_paZoneGroupKey &&
                    m_paDvaMessageKey== s.m_paDvaMessageKey &&
                    m_dvaMessageVersion== s.m_dvaMessageVersion &&
                    m_isEnabled== s.m_isEnabled);
        }
    };

    typedef std::list<PaAtsTriggeredBroadcastRecord> PaAtsTriggeredBroadcastRecords;
    typedef PaAtsTriggeredBroadcastRecords::iterator PaAtsTriggeredBroadcastRecordsIt;


    struct PaStationRecord
    {

        PaStationRecord()
            : m_pKeyOrId(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey())
        {}
        // Default assignment and copy constructors should be ok
	    unsigned long           m_pKeyOrId;
        unsigned long           m_locationKey;

        bool operator==(const PaStationRecord& s) const
        {
    	    return (m_pKeyOrId == s.m_pKeyOrId &&
                    m_locationKey  == s.m_locationKey);
        }
    };
    typedef std::list<PaStationRecord>      PaStationRecords;
    typedef PaStationRecords::iterator      PaStationRecordsIt;

} // end namespace TA_IRS_Bus

#endif // _CACHED_MAP_DEFINITIONS_HEADER_