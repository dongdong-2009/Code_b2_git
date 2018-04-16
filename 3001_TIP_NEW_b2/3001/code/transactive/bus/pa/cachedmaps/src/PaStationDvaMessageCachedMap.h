/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by: $Author: qi.huang $
  * 
  * Maintains a cache of Station Dva Message mappings, also listens
  * out for changes to database regarding the PA_DVA_MESSAGE table
  *
  */

#if !defined(PaStationDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaStationDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <list>

#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

namespace TA_Base_Core
{
    class IPaDvaMessage;
    class ConfigUpdateMessageSender;
}

namespace TA_IRS_Bus
{

    class PaStationDvaMessageCachedMap 
    : 
    public GenericCachedMap<StationDvaMessageRecord, TA_Base_Core::IPaDvaMessage>
    {
    public:
    

        PaStationDvaMessageCachedMap();
        ~PaStationDvaMessageCachedMap();

        /**
         * setStationDvaMessageRecord
         *
         * Updates internal dva message record map as appropriate to reflect the changes
         * defined in the input parameter and update the database (optional).  
         * 
         * If the record is new, the record.m_pKey must be set to -1 
         * and writeChangesToDatabase must be true.  Once the data is written to the database
         * this will update the record.m_pKey with the new pKey.  
         *
         * Note: Code will assert if record m_pKey == -1
         *          and writeChangesToDatabase == false
         * 
         * @param record the DVA message details.  Specify -1
         *        for the m_pKey field in the 
         *        case of a new record.  The m_pKey will then be returned modified.
         * @param writeChangesToDataBase set to true if the changes are to be written to 
         *        the database as well.  This should only be called by the PA Agent.
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @exception DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         *
         */
        void setStationDvaMessageRecord(StationDvaMessageRecord& record, bool writeChangesToDatabase = false);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException);


        /**
         * processDvaMessageUpdates
         *
         * Updates internal dva message record map as appropriate to reflect the changes
         * defined in the input parameter
         * 
         * @param updateData the updates information to refresh cached map with
         *
         * @return true if the data contained in 'updateData' is different from the data already contained
         *          within the cached map (that is, the we detected data was different)
         *
         * @exception CachedMappingNotFoundException if adhoc message ID couldn't be found
         *              in the dva message map (ie invalid or corrupt update structure)
         *
         */
        bool processDvaMessageUpdates(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromStationDvaMessageIdAndLocation
         *
         * Returns the key associated with the StationDvaMessageRecord matched by 
         * messageId and locationKey.
         *
         * pre: refreshKeyToStationDvaMessageRecordMap() has been called successfully.
         *
         * @param  messageId      the messageId field of StationDvaMessageRecord.
         * @param  locationKey the locationKey field of StationDvaMessageRecord.
         *
         * @return the primary key of the StationDvaMessageRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
         unsigned long getKeyFromStationDvaMessageIdAndLocation(unsigned long messageId, unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromStationDvaMessageIdAndLocation
         *
         * Returns the key associated with the StationDvaMessageRecord matched by 
         * messageId and locationKey.
         *
         * pre: refreshKeyToStationDvaMessageRecordMap() has been called successfully.
         *
         * @param  messageId      the messageId field of StationDvaMessageRecord.
         * @param  locationKey the locationKey field of StationDvaMessageRecord.
         *
         * @return the primary key of the StationDvaMessageRecord
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases
         *
         */
         StationDvaMessageRecord getStationDvaMessageRecordFromIdAndLocation(unsigned long messageId, unsigned long locationKey);

        /**
          * getClonedPrerecordedMessageKeys
          * 
          * Returns the pKeys of pre-recorded emergency  message records of 
          * the specified messageType.  The filter keyword is used to return 
          * only messages which contain the keyword (case insensitive).
          * 
          * @return the collection of StationDvaMessageRecord* for matching 
          *         pre-recorded message records.  
          *         *** THE CLIENT MUST DELETE THE OBJECTS ***
          *
          * @param  dvaSet specifies which set is of interest
          *
          * @param  messageType 'E' for emergency and 'N' for Normal
          *
          * @param  filterKeyword Optional keyword to filter messages by 
          *                       (case insensitive)
          *
          */
        std::list<StationDvaMessageRecord> getClonedPrerecordedMessageRecords
            ( const bool bIsOcc, const unsigned short messageType, const std::string& filterKeyword = "");

        /**
	      * getClonedAdhocMessages
	      * 
	      * Returns a collection of cloned StationDvaMessageRecord* of all ad-hoc 
          * message records belonging to the specified DVA set.
	      * 
          * @return the collection of cloned adhoc StationDvaMessageRecord*  
          *         *** THE CLIENT MUST DELETE THE OBJECTS ***
	      *
	      * @param  dvaSet specifies which set is of interest
  	      *
	      */
        std::list<StationDvaMessageRecord> getClonedAdhocMessageRecords();

        /** 
         * refreshKeyToStationDvaMessageRecordMap
         *
         * Reads the PA_DVA_MESSAGE table and populates the KeyToStationDvaMessageRecordMap collection
         * for all records matching  the optional locationKey.  To update the map with all records, 
         * do not specify the the parameter.
         *
         * @param  locationKey (optional) filters the PA_DVA_MESSAGE table on the LOCATIONKEY field.
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
        void refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey = 0);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);


    protected:

        /**
         * Virtual function extensions: We should return false (for changes made parameter) 
         *  if the item being updated doesn't map to our location
         */
        virtual bool processDatabaseRemovalNotification(unsigned long pKey);
        virtual bool processDatabaseUpdateCreateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const StationDvaMessageRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaDvaMessage* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual StationDvaMessageRecord 
                 createRecordFromInterface(TA_Base_Core::IPaDvaMessage& item);
                  //throw (TA_Base_Core::InvalidPasConfigurationException);

        /**
         * Called whenever there is a change being made by this class to the database
         *  - Note: Call AFTER the changes has been made
         *
         * This will post out ConfigUpdate messages to everyone listening out for
         *  configEditor updates (with relevant change information)
         *
         * @param recordKey the primary key of the record that has been changed
         *
         * @param modificationType the type of modification (Update, Create, Delete etc)
         *
         * @param changes list of the columns that have changed (if applicable)
         *
         */
        void submitConfigUpdate(unsigned long recordKey, 
                                TA_Base_Core::EModificationType modificationType, 
                                const std::vector<std::string>& changes);
    protected:

        // Sends out config update notifications whenever a change
        // made to database via this class (so other PAManagers know about it)
        TA_Base_Core::ConfigUpdateMessageSender* m_configUpdateSender;

    private:
        
        /// Location this cached map is operating for
        unsigned long m_locationKey;


    public: // Give public access to these filter classes

        /// Custom filter for getKeyFromStationDvaMessageIdAndLocation
        class FilterStationDvaMessageByIdAndLocation : public ICachedMapFilter<StationDvaMessageRecord>
        {
        public:
            FilterStationDvaMessageByIdAndLocation(unsigned long messageId, unsigned long locationKey);
        
            virtual bool isMatch(const StationDvaMessageRecord& record) const;
        
        private:
            unsigned long m_id;
            unsigned long m_locationKey;
        };

        class FilterStationDvaMessageIdRange
            : public ICachedMapFilter<StationDvaMessageRecord>
        {
        public:
            /**
             * FilterStationDvaMessageRange
             *
             * @param locationKey Defines location filtering, or 
             *      -1 for all locations
             * @param minId, maxId defines closed bounds for message ID range minId <= id <= maxId         
             */
            FilterStationDvaMessageIdRange(unsigned long locationKey, unsigned long minId, unsigned long maxId);

            virtual bool isMatch(const StationDvaMessageRecord& record) const;

        private:   
            unsigned long m_locationKey, m_minId, m_maxId;
        };
    };


} // end namespace TA_IRS_Bus

#endif //!defined(PaStationDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
