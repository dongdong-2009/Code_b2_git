/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of PA ATS Triggered Broadcast mappings, also listens
  * out for changes to database regarding the PA_ATS_TRIGGERED_BROADCAST
  * table
  *
  */


#if !defined(PaAtsTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaAtsTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_


#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"

#include <list>

namespace TA_IRS_Bus
{

    class PaAtsTriggeredBroadcastCachedMap 
    : 
    public GenericCachedMap<PaAtsTriggeredBroadcastRecord, TA_Base_Core::IPaAtsTriggeredBroadcast>
    {
    public:
    
        PaAtsTriggeredBroadcastCachedMap();

        /** 
         * refreshKeyToPaAtsTriggeredBroadcastRecordMap
         *
         * Reads the PA_ATS_TRIGGERED_BROADCAST table and populates the internal map
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
        void refreshKeyToPaAtsTriggeredBroadcastRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getAtsTriggeredBroadcastRecordFromDetail (passthrough function)
         *
         * Searches and returns the record which matches the provided parameters.
         * If a matching record is not found, then an empty struct will be returned
         * with the isEnabled flag set to false.  The client code should ignore 
         * this.  The usual CachedMappingNotFoundException is not thrown for 
         * performance reasons.  
         *
         * @param  eventType the enum type of the ATS event.  Please refer to the 
         *         TA_IRS_Bus::IAtsPasCorbaDef::EEventType for details
         * @param  locationKey the source location of event
         * @param  destinationLocationKey the destinate location of the event
         * @param  platformId the platform id of the event
         * @param  isLastTrain whether the train in the event is flagged as the
         *         last train of the day
         *
         * @return the matching PaAtsTriggeredBroadcastRecord struct
         *
         */


        PaAtsTriggeredBroadcastRecord getAtsTriggeredBroadcastRecordFromDetail(
            unsigned long eventType, 
            unsigned long locationKey, 
            unsigned long destinationLocationKey,
            unsigned long platformId,
            bool          isLastTrain);

    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const PaAtsTriggeredBroadcastRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaAtsTriggeredBroadcast* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual PaAtsTriggeredBroadcastRecord 
                 createRecordFromInterface(TA_Base_Core::IPaAtsTriggeredBroadcast& item);
                  //throw (TA_Base_Core::InvalidPasConfigurationException);

    private:

        /// Custom filter for getAtsTriggeredBroadcastRecordFromDetail
        class FilterATSRecordByDetail
            : public ICachedMapFilter<PaAtsTriggeredBroadcastRecord>
        {
        public:
            FilterATSRecordByDetail(unsigned long eventType, 
                                    unsigned long locationKey, 
                                    unsigned long destinationLocationKey,
                                    unsigned long platformId,
                                    bool          isLastTrain);

            virtual bool isMatch(const PaAtsTriggeredBroadcastRecord& record) const;
        
        private:
    
            unsigned long m_eventType;
            unsigned long m_locationKey;
            unsigned long m_destinationLocationKey;
            unsigned long m_platformId;
            bool          m_isLastTrain;
        };
    };

} // end namespace TA_IRS_Bus

#endif //!defined(PaAtsTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
