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


#if !defined(PaFasTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaFasTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_


#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"

#include <list>

namespace TA_IRS_Bus
{

    class PaFasTriggeredBroadcastCachedMap 
    : 
    public GenericCachedMap<PaFasTriggeredBroadcastRecord, TA_Base_Core::IPaFasTriggeredBroadcast>
    {
    public:
    
        PaFasTriggeredBroadcastCachedMap();

        /** 
         * refreshKeyToPaFasTriggeredBroadcastRecordMap
         *
         * Reads the PA_FAS_TRIGGERED_BROADCAST table and populates the internal map
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
        void refreshKeyToPaFasTriggeredBroadcastRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getFasTriggeredBroadcastRecordFromDetail (passthrough function)
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
         * @return the matching PaFasTriggeredBroadcastRecord struct
         *
         */


        PaFasTriggeredBroadcastRecord getFasTriggeredBroadcastRecordFromDetail( unsigned long ulAlarmEntityKey );

    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const PaFasTriggeredBroadcastRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaFasTriggeredBroadcast* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual PaFasTriggeredBroadcastRecord 
                 createRecordFromInterface(TA_Base_Core::IPaFasTriggeredBroadcast& item);
                  //throw (TA_Base_Core::InvalidPasConfigurationException);

    private:

        /// Custom filter for getFasTriggeredBroadcastRecordFromDetail
        class FilterATSRecordByDetail
            : public ICachedMapFilter<PaFasTriggeredBroadcastRecord>
        {
        public:
            FilterATSRecordByDetail(unsigned long ulAlarmEntityKey);

            virtual bool isMatch(const PaFasTriggeredBroadcastRecord& record) const;
        
        private:
    
            unsigned long m_ulFasAlarmEntityKey;
        };
    };

} // end namespace TA_IRS_Bus

#endif //!defined(PaFasTriggeredBroadcastCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
