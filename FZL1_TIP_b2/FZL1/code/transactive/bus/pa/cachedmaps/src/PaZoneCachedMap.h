/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/PaZoneCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_ZONE table
  *
  */

#if !defined(PaZoneCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaZoneCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <list>
#include <set>
#include <vector>

#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/pa/cachedmaps/src/ILocationNameHelper.h"

namespace TA_Base_Core
{
    class IPaZone;
}

namespace TA_IRS_Bus
{
    class PaZoneCachedMap 
    : 
    public GenericCachedMap<PaZoneRecord, TA_Base_Core::IPaZone>
    {
    public:
    
        PaZoneCachedMap(ILocationNameHelper& locationNameHelper);

        /** 
         * refreshKeyToRecordMap
         *
         * Reads the PA_ZONE table and populates the record map collection
         * for all records matching the optional locationKey.  To update the map with all records, 
         * do not specify the the parameter.
         *
         * @param  locationKey (optional) filters the table on the LOCATIONKEY field.
         *
         * @exception DatabaseException 
         *         If there is a problem establishing a connection with the database.
         *         If an error is encountered while retrieving data.
         * @exception DataException      
         *         If the data cannot be converted to the required format.
         *         If the wrong ammount of data is retrieved.
         * @exception InvalidPasConfigurationException 
         *         If a single key is mapped to more than one record (i.e. pkeys are not unique).  
         *         This indicates invalid configuration in the databases.
         *
         */
        unsigned long refreshKeyToPaZoneRecordMap(unsigned long locationKey = TA_Base_Core::DatabaseKey::getInvalidKey()) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getKeyFromPaZoneIdAndLocation
         *
         * Returns the key associated with the PaZoneRecord match by zoneId and locationKey.
         * This should be unique.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  zoneId      the zone Id field of PaZoneRecord.
         * @param  locationKey the locationKey field of PaZoneRecord.
         *
         * @return the primary key of the PaZoneRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
         unsigned long getKeyFromPaZoneIdAndLocation(unsigned long zoneId, unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

         /**
          * getPaZoneRecordsByGroupId
          *
          * Performs a database lookup to find all the records matching the input group ID
          * (looking in the table PA_ZONE_GROUP_HELPER)
          *
          * @return the subset of PA zone records contained in this class that have
          *         a group ID matching the input group identifier
          *
         * @exception DatabaseException 
         *         If there is a problem establishing a connection with the database.
         *         If an error is encountered while retrieving data.
         * @exception DataException      
         *         If the data cannot be converted to the required format.
         *         If the wrong ammount of data is retrieved.
         * @exception CachedMappingNotFoundException 
         *         If the configuration is invalid (the group may map to a Pa Zone record that is
         *          outside the scope of this classes cached data)
         *
         */
        PaZoneCachedMap::DbRecordTypeSet getPaZoneRecordsByGroupId(unsigned long groupId);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getRecordFromEquipmentEntityName
         *
         * Returns the zoneId associated with the equipment datanode entity key in 
         * the PA zone record.  This should be unique.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  equipmentEntityKey the equipmentEntityKey field of PaZoneRecord.
         *
         * @return the zoneId of the PaZoneRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
        PaZoneRecord getRecordFromEquipmentEntityName(const std::string& equipmentEntityName);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getRecordSimilarToEquipmentEntityName
         *
         * Returns the zoneId associated with the equipment datanode entity key in 
         * the PA zone record.  This should be unique.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  equipmentEntityKey the equipmentEntityKey field of PaZoneRecord.
         *
         * @return the zoneId of the PaZoneRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
        PaZoneRecords getRecordsSimilarToEquipmentEntityName(const bool bOcc, const std::string& equipmentEntityName);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getRecordSimilarToEquipmentEntityNames
         *
         * Returns the zoneId associated with the equipment datanode entity key in 
         * the PA zone record.  This should be unique.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  equipmentEntityKey the equipmentEntityKey field of PaZoneRecord.
         *
         * @return the zoneId of the PaZoneRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
        PaZoneRecords getRecordsSimilarToEquipmentEntityNames(const bool bOcc, const std::set<std::string>& zoneNames);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getPaZoneRecordsFromArea
         *
         * Returns all PaZoneRecords matching the area parameter.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  area the area field of PaZoneRecord.
         *
         * @return a list of matching PaZoneRecords
         *
         */
        PaZoneRecords getPaZoneRecordsFromArea(const std::string& area);

        /** 
         * getPaZoneRecordsFromLocationKey
         *
         * Returns all PaZoneRecords matching the locationKey parameter.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  locationKey the locationkey field of PaZoneRecord.
         *
         * @return a list of matching PaZoneRecords
         *
         */
        PaZoneRecords getPaZoneRecordsFromLocationKey(unsigned long locationKey);
    

        /**
         * getZoneIdsUsingFilter
         *
         * @return an array of primary key indices for Pa Zones, for all Pa Zones
         *          matching the input filter
         *
         * @param filter the filter used to identify which Pa Zones to return, null
         *          to return all
         *
         * @see GenericCachedMap::getAllRecords
         *
         */
        std::vector<unsigned long> getZoneIdsUsingFilter(ICachedMapFilter<PaZoneRecord>* filter);

    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const PaZoneRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaZone* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual PaZoneRecord createRecordFromInterface(TA_Base_Core::IPaZone& item);
                    //throw (TA_Base_Core::InvalidPasConfigurationException);


    private:


        /// Custom filter for getKeyFromPaZoneIdAndLocation
        class FilterPaZoneByIdAndLocation : public ICachedMapFilter<PaZoneRecord>
        {
        public:
            FilterPaZoneByIdAndLocation(unsigned long zoneId, 
                                        unsigned long locationKey);

            virtual bool isMatch(const PaZoneRecord& record) const;
        
        private:
            unsigned long m_zoneId;
            unsigned long m_locationKey;
        };

        /// Custom filter for getRecordFromEquipmentEntityName        
        class FilterPaZoneByEquipmentEntityName : public ICachedMapFilter<PaZoneRecord>
        {
        public:
            FilterPaZoneByEquipmentEntityName(const std::string& equipmentEntityName);

            virtual bool isMatch(const PaZoneRecord& record) const;
        
        private:
            std::string m_equipmentEntityName;
        };


        /// Custom filter for FilterPaZoneSimilarToEquipmentEntityName        
        class FilterPaZoneSimilarToEquipmentEntityName : public ICachedMapFilter<PaZoneRecord>
        {
        public:
            FilterPaZoneSimilarToEquipmentEntityName(const bool bOcc, const std::set<std::string>& equipmentEntityNames);

            virtual bool isMatch(const PaZoneRecord& record) const;

        private:
            bool m_bOcc;
            typedef std::vector<std::string> ZoneNameTokenVec;
            std::vector<ZoneNameTokenVec> m_vecSplittedZoneName;
        };

        /// Custom filter for getPaZoneRecordsFromArea
        class FilterPaZoneByArea : public ICachedMapFilter<PaZoneRecord>
        {
        public:
            FilterPaZoneByArea(const std::string& area) : m_area(area)
            {}

            virtual bool isMatch(const PaZoneRecord& record) const
            { return record.m_area == m_area; }
        
        private:
            std::string m_area;
        };

        /// Custom filter for getPaZoneRecordsFromLocationKey
        class FilterPaZoneByLocationKey : public ICachedMapFilter<PaZoneRecord>
        {
        public:
            FilterPaZoneByLocationKey(unsigned long locationKey) : m_locationKey(locationKey) 
            {}

            virtual bool isMatch(const PaZoneRecord& record) const
            { return record.m_locationKey == m_locationKey; }
        
        private:
            unsigned long m_locationKey;
        };

        ILocationNameHelper& m_locationNameHelper;
    };

} // end namespace TA_IRS_Bus
    
#endif //!defined(PaZoneCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
