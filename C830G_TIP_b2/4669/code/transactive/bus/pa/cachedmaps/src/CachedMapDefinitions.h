/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/CachedMapDefinitions.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
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
        StationDvaMessageRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_id(0),
              m_type(0),
              m_tisMessageTag(-1),
              m_tisLibrarySection(-1),
              m_tisLibraryVersion(-1),
              m_label("")
        {}
        // Default assignment and copy constructors should be ok
        unsigned long m_pKey;
        unsigned long m_locationKey;
	    unsigned long m_id;
	    std::string   m_label;
	    char          m_type;
	    int           m_tisMessageTag;
        int           m_tisLibrarySection;
        unsigned long m_tisLibraryVersion;

        bool operator==(const StationDvaMessageRecord& s) const
        {
            return (m_pKey == s.m_pKey &&
                    m_locationKey == s.m_locationKey &&
    	            m_id == s.m_id &&
    	            m_label == s.m_label &&
    	            m_type == s.m_type &&
    	            m_tisMessageTag == s.m_tisMessageTag &&
                    m_tisLibrarySection == s.m_tisLibrarySection &&
                    m_tisLibraryVersion == s.m_tisLibraryVersion);
        }

        std::string getTypeAsString() const
        {
            switch (m_type)
            {
            case 'N':
                return "Normal";
            case 'E':
                return"Emergency";
            case 'X':
                return "Deleted";
            default:
                return "Unknown";
            }
        }
    };

    typedef std::list<StationDvaMessageRecord*>         StationDvaMessageRecordPointers;
    typedef StationDvaMessageRecordPointers::iterator   StationDvaMessageRecordPointersIt;

    typedef std::list<StationDvaMessageRecord>   StationDvaMessageRecords;
    typedef StationDvaMessageRecords::iterator   StationDvaMessageRecordsIt;

    struct PaZoneRecord
    {
        /// Defines where certain parameter extracted from entityname (from database)
        /// - used to define m_area and m_zoneShortName
        enum ETokenIdentifier
        {
            STATION = 0,
            AREA = 2,
            ZONESHORTNAME = 3
        };

        PaZoneRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_id(0),
              // liuyu++ TD8849
              m_orderId(TA_Base_Core::DatabaseKey::getInvalidKey()),
              // ++liuyu TD8849
              //m_entityKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_stnEquipmentEntityName(""),
              m_occEquipmentEntityName(""),
              m_locationName(""),
              m_occLocationName(""),
              m_label(""),
              m_area(""),
              m_occZoneShortName(""),
              m_stnZoneShortName("")
        {}
        // Default assignment and copy constructors should be ok
	    unsigned long           m_pKey;
        unsigned long           m_locationKey;
        std::string             m_locationName;
        unsigned long           m_id;
        // liuyu++ TD8849
        unsigned long           m_orderId;
        // ++liuyu TD8849
	    std::string             m_label;
	    std::string             m_area;
        
        // We can derive this information from equipment entity name, 
		// stored purely for (filtering) efficiency reasons            
        std::string             m_occZoneShortName;
        std::string             m_occLocationName;        
        std::string             m_stnZoneShortName;

        //unsigned long           m_entityKey;
        std::string             m_stnEquipmentEntityName;
        std::string             m_occEquipmentEntityName;
        std::list<std::string>  m_equipmentEntities;

        bool operator==(const PaZoneRecord& s) const
        {
    	    return (m_pKey == s.m_pKey &&
                    m_locationKey  == s.m_locationKey &&
                    m_locationName  == s.m_locationName &&
                    m_occLocationName == s.m_occLocationName &&
                    m_id  == s.m_id &&
                    // liuyu++ TD8849
                    m_orderId == s.m_orderId &&
                    // ++liuyu TD8849
                    m_label  == s.m_label &&
                    m_area  == s.m_area &&
                    m_stnZoneShortName  == s.m_stnZoneShortName &&
                    m_occZoneShortName  == s.m_occZoneShortName &&
                    //m_entityKey  == s.m_entityKey &&
                    m_stnEquipmentEntityName == s.m_stnEquipmentEntityName &&
                    m_occEquipmentEntityName == s.m_occEquipmentEntityName &&
                    m_equipmentEntities  == s.m_equipmentEntities);
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
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_privateAdhoc1(""),
              m_privateAdhoc2(""),
              m_privateAdhoc3(""),
              m_privateAdhoc4(""),
              m_privateAdhoc5(""),
              m_privateAdhoc6(""),
              m_privatePreRecorded(""),
              m_publicAdhoc1(""),
              m_publicAdhoc2(""),
              m_publicAdhoc3(""),
              m_publicAdhoc4(""),
              m_publicAdhoc5(""),
              m_publicAdhoc6(""),
              m_publicPreRecorded("")
        {}

        // Default assignment and copy constructors should be ok
        unsigned long m_pKey;
        unsigned long m_locationKey;

        std::string   m_privateAdhoc1;
        std::string   m_privateAdhoc2;
        std::string   m_privateAdhoc3;
        std::string   m_privateAdhoc4;
        std::string   m_privateAdhoc5;
        std::string   m_privateAdhoc6;
        std::string   m_privatePreRecorded;

        std::string   m_publicAdhoc1;
        std::string   m_publicAdhoc2;
        std::string   m_publicAdhoc3;
        std::string   m_publicAdhoc4;
        std::string   m_publicAdhoc5;
        std::string   m_publicAdhoc6;
        std::string   m_publicPreRecorded;

        bool operator==(const DvaVersionRecord& s) const
        {
            return (m_pKey == s.m_pKey &&
                    m_locationKey == s.m_locationKey &&
            
                    m_privateAdhoc1 == s.m_privateAdhoc1 &&
                    m_privateAdhoc2 == s.m_privateAdhoc2 &&
                    m_privateAdhoc3 == s.m_privateAdhoc3 &&
                    m_privateAdhoc4 == s.m_privateAdhoc4 &&
                    m_privateAdhoc5 == s.m_privateAdhoc5 &&
                    m_privateAdhoc6 == s.m_privateAdhoc6 &&
                    m_privatePreRecorded == s.m_privatePreRecorded &&
            
                    m_publicAdhoc1 == s.m_publicAdhoc1 &&
                    m_publicAdhoc2 == s.m_publicAdhoc2 &&
                    m_publicAdhoc3 == s.m_publicAdhoc3 &&
                    m_publicAdhoc4 == s.m_publicAdhoc4 &&
                    m_publicAdhoc5 == s.m_publicAdhoc5 &&
                    m_publicAdhoc6 == s.m_publicAdhoc6 &&
                    m_publicPreRecorded == s.m_publicPreRecorded);        
        }
    };
    typedef std::list<DvaVersionRecord>      DvaVersionRecords;
    typedef DvaVersionRecords::iterator      DvaVersionRecordsIt;

    struct DvaPublicVersionRecord
    {
        DvaPublicVersionRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_publicAdhoc1(""),
              m_publicAdhoc2(""),
              m_publicAdhoc3(""),
              m_publicAdhoc4(""),
              m_publicAdhoc5(""),
              m_publicAdhoc6(""),
              m_publicPreRecorded("")
        {}

        // Default assignment and copy constructors should be ok
        unsigned long m_pKey;
        unsigned long m_locationKey;

        std::string   m_publicAdhoc1;
        std::string   m_publicAdhoc2;
        std::string   m_publicAdhoc3;
        std::string   m_publicAdhoc4;
        std::string   m_publicAdhoc5;
        std::string   m_publicAdhoc6;
        std::string   m_publicPreRecorded;

        bool operator==(const DvaPublicVersionRecord& s) const
        {
            return (m_pKey == s.m_pKey &&
                    m_locationKey == s.m_locationKey &&
            
                    m_publicAdhoc1 == s.m_publicAdhoc1 &&
                    m_publicAdhoc2 == s.m_publicAdhoc2 &&
                    m_publicAdhoc3 == s.m_publicAdhoc3 &&
                    m_publicAdhoc4 == s.m_publicAdhoc4 &&
                    m_publicAdhoc5 == s.m_publicAdhoc5 &&
                    m_publicAdhoc6 == s.m_publicAdhoc6 &&
                    m_publicPreRecorded == s.m_publicPreRecorded);        
        }
    };
    typedef std::list<DvaPublicVersionRecord>      DvaPublicVersionRecords;
    typedef DvaPublicVersionRecords::iterator      DvaPublicVersionRecordsIt;


    struct DvaPrivateVersionRecord
    {
        DvaPrivateVersionRecord()
            : m_pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
              m_privateAdhoc1(""),
              m_privateAdhoc2(""),
              m_privateAdhoc3(""),
              m_privateAdhoc4(""),
              m_privateAdhoc5(""),
              m_privateAdhoc6(""),
              m_privatePreRecorded("")
        {}

        // Default assignment and copy constructors should be ok
        unsigned long m_pKey;
        unsigned long m_locationKey;

        std::string   m_privateAdhoc1;
        std::string   m_privateAdhoc2;
        std::string   m_privateAdhoc3;
        std::string   m_privateAdhoc4;
        std::string   m_privateAdhoc5;
        std::string   m_privateAdhoc6;
        std::string   m_privatePreRecorded;

        bool operator==(const DvaPrivateVersionRecord& s) const
        {
            return (m_pKey == s.m_pKey &&
				m_locationKey == s.m_locationKey &&
				
				m_privateAdhoc1 == s.m_privateAdhoc1 &&
				m_privateAdhoc2 == s.m_privateAdhoc2 &&
				m_privateAdhoc3 == s.m_privateAdhoc3 &&
				m_privateAdhoc4 == s.m_privateAdhoc4 &&
				m_privateAdhoc5 == s.m_privateAdhoc5 &&
				m_privateAdhoc6 == s.m_privateAdhoc6 &&
				m_privatePreRecorded == s.m_privatePreRecorded);        
        }
    };
    typedef std::list<DvaPrivateVersionRecord>      DvaPrivateVersionRecords;
    typedef DvaPrivateVersionRecords::iterator      DvaPrivateVersionRecordsIt;


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