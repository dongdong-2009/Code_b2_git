/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/PaStationCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_STATION table
  *
  */

#if !defined(PaStationCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaStationCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <list>

#include "core/data_access_interface/pa/src/PaStationAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"

namespace TA_Base_Core
{
    class IPaStation;
}

namespace TA_IRS_Bus
{

    class PaStationCachedMap : public GenericCachedMap<PaStationRecord, TA_Base_Core::IPaStation>
    {
    public:
    
        PaStationCachedMap();

        /** 
         * refreshKeyToRecordMap
         *
         * Reads the PA_STATION table and populates the record map collection
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
        unsigned long refreshKeyToPaStationRecordMap(unsigned long locationKey = TA_Base_Core::DatabaseKey::getInvalidKey()) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getPasStationIdFromLocationKey
         *
         * Returns the key (station Id) associated with the PaStationRecord 
         * matched by locationKey.   This should be unique.
         *
         * pre: refreshKeyToPaStationRecordMap() has been called successfully.
         *
         * @param  locationKey the locationKey field of PaStationRecord.
         *
         * @return the primary key (station Id) of the PaStationRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
         unsigned long getPasStationIdFromLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getLocationKeyFromPasStationId
         *
         * Returns the location key associated with the Station Id (or pKey). 
         * This should be unique.
         *
         * pre: refreshKeyToPaStationRecordMap() has been called successfully.
         *
         * @param  the primary key (station Id) of the PaStationRecord
         *
         * @return locationKey the locationKey field of PaStationRecord.
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
         unsigned long getLocationKeyFromPasStationId(unsigned long stationId);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

         void absoluteToRelativeZoneId(const unsigned long absoluteZoneId, 
                                       unsigned long& relativeZoneId, 
                                       unsigned long& locationKey);
             //throw(TA_Base_Core::CachedMappingNotFoundException);

         void relativeToAbsoluteZoneId(unsigned long& absoluteZoneId,
                                       const unsigned long relativeZoneId, 
                                       const unsigned long locationKey);
             //throw(TA_Base_Core::CachedMappingNotFoundException);


    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const PaStationRecord& paStation);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaStation* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual PaStationRecord createRecordFromInterface(TA_Base_Core::IPaStation& item);
                    //throw (TA_IRS_Core::InvalidPasConfigurationException);

        /// Custom filter for getDvaVersionRecordFromLocationKey
        class FilterByLocationKey
            : public ICachedMapFilter<PaStationRecord>
        {
        public:
            FilterByLocationKey(unsigned long locationKey) : m_locationKey(locationKey) 
            {}

            virtual bool isMatch(const PaStationRecord& record) const
            { return record.m_locationKey == m_locationKey; }
        
        private:
    
            unsigned long m_locationKey;
        };

    };

} // end namespace TA_IRS_Bus
    
#endif //!defined(PaStationCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
