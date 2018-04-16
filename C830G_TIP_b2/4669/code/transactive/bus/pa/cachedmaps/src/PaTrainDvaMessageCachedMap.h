/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Train Dva Message mappings, also listens
  * out for changes to database regarding the PA_TRAIN_DVA_MESSAGE table
  *
  */


#if !defined(PaTrainDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaTrainDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_


#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"

#include <list>

namespace TA_Base_Core
{
    class IPaTrainDvaMessage;
}

namespace TA_IRS_Bus
{
    class PaTrainDvaMessageCachedMap 
    : 
    public GenericCachedMap<TrainDvaMessageRecord, TA_Base_Core::IPaTrainDvaMessage>
    {
    public:
        
        class IGlobalTrainVersionObserver
        {
        public:
            /**
             * onGlobalTrainVersionChanged
             *
             * Called whenever there is a change in the global train version setting
             */
            virtual void onGlobalTrainVersionChanged() = 0;
        };

        PaTrainDvaMessageCachedMap();
        virtual ~PaTrainDvaMessageCachedMap();

        /** 
         * refreshKeyToTrainDvaMessageRecordMap
         *
         * Reads the PA_DVA_MESSAGE table and populates the KeyToTrainDvaMessageRecordMap collection.
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
        void refreshKeyToTrainDvaMessageRecordMap() ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /**
         * getTrainDvaMessageRecords
         *
         * @return set of train DVA messages matching the input criteria
         *          (returns all records if no criteria specified)
         *
         * @param  filterKeyword (Optional) keyword to filter messages by 
         *                       (case insensitive)
         */
        TrainDvaMessageRecords getTrainDvaMessageRecords(const std::string& filterKeyword = "");
                        //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * startListening (base class extension)
         * @see GenericCachedMapBase::startListening for more details
         */
        virtual void startListening();

        /**
         * processUpdate (base class extension)
         * @see GenericCachedMapBase::processUpdate for more details
         * 
         * Intercepts messages at top level to respond to the global parameter
         *  change message (with a primary key of -1)
         */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
        
        /**
         * registerGlobalVersionObserver
         *
         * Adds a new observer that watches out for changes to the global train version
         *
         * @param ob the observer that is to receive notifications when a change
         *          takes place (database change notification)
         *
         * @pre the observer 'ob' hasn't already been registered, and is not null
         *
         * @see deRegisterGlobalVersionObserver
         *
         */
        void registerGlobalVersionObserver(IGlobalTrainVersionObserver* ob);
    
        /**
         * deRegisterGlobalVersionObserver
         *
         * Removes the specified observer that was added with registerObserver
         *
         * @param ob the observer we want to remove
         *
         * @pre the specified observer had already been added with registerObserver
         *
         * @see registerGlobalVersionObserver
         *
         */
        void deRegisterGlobalVersionObserver(IGlobalTrainVersionObserver* ob);

    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const TrainDvaMessageRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaTrainDvaMessage* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual TrainDvaMessageRecord 
                 createRecordFromInterface(TA_Base_Core::IPaTrainDvaMessage& item);
                  //throw (TA_Base_Core::InvalidPasConfigurationException);

    private:
    
        std::vector<IGlobalTrainVersionObserver*> m_registeredGlobalVersionObservers;
        TA_Base_Core::NonReEntrantThreadLockable     m_updateGlobalVersionObserverLock;
    };

} // end namespace TA_IRS_Bus

#endif //!defined(PaTrainDvaMessageCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
