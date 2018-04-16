/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/CachedMaps.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Maintains all the cached maps for PA applications
  *
  */

#if !defined(PA_CACHED_MAPS_INCLUDED)
#define PA_CACHED_MAPS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessageVersion.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "bus/pa/cachedmaps/src/PaStationCachedMap.h"
#include "bus/pa/cachedmaps/src/ILocationNameHelper.h"

#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePublicVersion.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h"

typedef std::list<unsigned long>                Keys;
typedef Keys::iterator                          KeysIt;

// This is required to tell the compiler that UnitTestAccessBridge is not part
// of the TA_IRS_Bus namespace (you will get ambiguity compiler errors otherwise)
class UnitTestAccessBridge;

namespace TA_Base_Bus
{
	class DataPoint;	
}

namespace TA_IRS_Bus
{   

    // Do not include these in header, too much load on compiler (error C1076)
    class PaZoneCachedMap;
    class PaZoneGroupCachedMap;
    class PaStationDvaMessageCachedMap;
    class PaTrainDvaMessageCachedMap;
    class PaDvaVersionCachedMap;
	class PaDvaPublicVersionCachedMap;
	class PaDvaPrivateVersionCachedMap;
    
    class ICachedMapObserver;

    class CachedMaps
    {
        friend class UnitTestAccessBridge;

    protected:

        // Each of the cached map collections    
        typedef std::map<unsigned long, std::string>                LocationKeyToNameMap;
        typedef LocationKeyToNameMap::iterator                      LocationKeyToNameMapIt;

        typedef std::map<int, TA_Base_Bus::DataPoint*>                           ZoneIdToDataPointMap;
        typedef ZoneIdToDataPointMap::iterator                      ZoneIdToDataPointMapIt;

    public:

        /**
         * getInstance
	     *
	     * Returns an instance of the class
         *
         * @return the pointer to this singleton instance
         *
         */
	    static CachedMaps* getInstance();

        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         *
         */
        static void removeInstance();


    ///////////////////////////////////////////////////////////////////////////
    //
    //                 Ats Triggered Broadcast Access
    //
    ///////////////////////////////////////////////////////////////////////////

        /** 
         * refreshKeyToPaAtsTriggeredBroadcastRecordMap (passthrough function)
         *
         * @see PaAtsTriggeredBroadcastCachedMap::refreshKeyToPaAtsTriggeredBroadcastRecordMap
         *      for more details
         *
         */
        void refreshKeyToPaAtsTriggeredBroadcastRecordMap(unsigned long locationKey = -1) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
        {
            m_paAtsTriggeredBroadcastCachedMap->refreshKeyToPaAtsTriggeredBroadcastRecordMap(locationKey);
        }

        /** 
         * getAtsTriggeredBroadcastRecordFromDetail (passthrough function)
         *
         * @see PaAtsTriggeredBroadcastCachedMap::getAtsTriggeredBroadcastRecordFromDetail
         *      for more details
         *
         */


        PaAtsTriggeredBroadcastRecord getAtsTriggeredBroadcastRecordFromDetail(
            unsigned long eventType, 
            unsigned long locationKey, 
            unsigned long destinationLocationKey,
            unsigned long platformId,
            bool          isLastTrain) 
        {
            return m_paAtsTriggeredBroadcastCachedMap->getAtsTriggeredBroadcastRecordFromDetail(
                eventType,
                locationKey,
                destinationLocationKey,
                platformId,
                isLastTrain);
        }

    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Train DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////
        /** 
         * refreshKeyToTrainDvaMessageRecordMap (passthrough function)
         *
         * @see PaTrainDvaMessageCachedMap::refreshKeyToTrainDvaMessageRecordMap for more details
         *
         */
        void refreshKeyToTrainDvaMessageRecordMap(); 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getTrainDvaMessageRecordFromKey (passthrough function)
         *
         * @see GenericCachedMap::getCachedDbRecordFromKey for more details
         *
         */
	     TrainDvaMessageRecord getTrainDvaMessageRecordFromKey(unsigned long key);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getTrainDvaMessageRecords (passthrough function)
         *
         * @see PaTrainDvaMessageCachedMap::getAllRecords for more details
         *
         */
	     TrainDvaMessageRecords getTrainDvaMessageRecords(const std::string& filterKeyword = "");

    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////


        /** 
         * refreshKeyToStationDvaMessageRecordMap (passthrough function)
         *
         * @see GenericCachedMap::refreshKeyToStationDvaMessageRecordMap for more details
         *
         */
        void refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);


        /** 
         * getStationDvaMessageRecordFromKey (passthrough function)
         *
         * @see GenericCachedMap::getCachedDbRecordFromKey for more details
         *
         */                      
	     StationDvaMessageRecord getStationDvaMessageRecordFromKey(unsigned long key);
            //throw(TA_Base_Core::CachedMappingNotFoundException);
    
        /** 
         * getStationDvaMessageIdFromKey
         *
         * Returns the StationDvaMessageId associated with the key.
         *
         * pre: refreshKeyToStationDvaMessageRecordMap() has been called successfully.
         *
         * @param  key the primary key of the StationDvaMessageRecord.
         *
         * @return the StationDvaMessageId
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	     unsigned long getStationDvaMessageIdFromKey(unsigned long key);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getStationDvaMessageLocationKeyFromKey
         *
         * Returns the StationDvaMessageRecord locationKey associated with the key.
         *
         * pre: refreshKeyToStationDvaMessageRecordMap() has been called successfully.
         *
         * @param  key the primary key of the StationDvaMessageRecord.
         *
         * @return the StationDvaMessageRecord locationKey
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	     unsigned long getStationDvaMessageLocationKeyFromKey(unsigned long key);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromStationDvaMessageIdAndLocation (passthrough function)
         *
         * @see PaStationDvaMessageCachedMap::getKeyFromStationDvaMessageIdAndLocation
         *
         */
         unsigned long getKeyFromStationDvaMessageIdAndLocation(unsigned long messageId, unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
          * getClonedAdhocMessageRecords (pass through function)
          * 
          * @see PaStationDvaMessageCachedMap::getClonedAdhocMessageRecords
          *
          */
        StationDvaMessageRecordPointers getClonedAdhocMessageRecords
            ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet );
	    
	    
        /**
          * getClonedPrerecordedMessageKeys (pass through function)
          * 
          * @see PaStationDvaMessageCachedMap::getClonedPrerecordedMessageRecords
          *
          */
        StationDvaMessageRecordPointers getClonedPrerecordedMessageRecords
            ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet, 
              char messageType,
              const std::string& filterKeyword = "");


        /**
         * processDvaMessageUpdates (passthrough function)
         *
         * @see PaStationDvaMessageCachedMap::processDvaMessageUpdates
         *
         */
        bool processDvaMessageUpdates(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData);
            //throw(TA_Base_Core::CachedMappingNotFoundException);


        /**
         * getStationDvaMessageSetFromKey
         *
         * Returns the DVA set which the message key belongs.  If it cannot be classified, 
         * the set returned is SetUnknown
         * 
         * @param messageKey primary key of the DVA message
         *
         * @return the enumerated type for the DVA set
         *
         * @exception CachedMappingNotFoundException if the message ID couldn't be found
         *            in the dva message map.
         *
         */
        TA_Base_Bus::IPAAgentCorbaDef::EDvaSet getStationDvaMessageSetFromKey(unsigned long messageKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * isPreRecordedMessage
         *
         * Returns true if the DVA message key is a pre-recorded message
         * 
         * @param messageKey primary key of the DVA message
         *
         * @return true if the DVA message key is a pre-recorded message
         *
         * @exception CachedMappingNotFoundException if the message ID couldn't be found
         *            in the dva message map.
         *
         */
        bool isPreRecordedMessage(unsigned long messageKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * isAdhocMessage
         *
         * Returns true if the DVA message key is an ad-hoc message.  Currently
         * messages can only be either pre-recorded or ad-hoc, so this will 
         * call isPreRecordedMessage() and return the inverse.
         * 
         * @param messageKey primary key of the DVA message
         *
         * @return true if the DVA message key is a ad-hoc message
         *
         * @exception CachedMappingNotFoundException if the message ID couldn't be found
         *            in the dva message map.
         *
         */
        bool isAdhocMessage(unsigned long messageKey)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
        { return !isPreRecordedMessage(messageKey); }

        /**
         * setStationDvaMessageRecord (passthrough function)
         *
         * @see PaStationDvaMessageCachedMap::setStationDvaMessageRecord for more details
         *
         */
        void setStationDvaMessageRecord(StationDvaMessageRecord& record, bool writeChangesToDatabase = false);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException);


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

        /** 
         * refreshKeyToDvaVersionRecordMap
         *
         * @see PaDvaVersionCachedMap::refreshKeyToRecordMap for more details
         *
         */
        void refreshKeyToDvaVersionRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getDvaVersionRecordFromLocationKey
         *
         * @see PaDvaVersionCachedMap::getRecordFromLocation for more details
         */
        DvaVersionRecord getDvaVersionRecordFromLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromDvaVersionRecordLocationKey
         *
         * @see PaDvaVersionCachedMap::getKeyFromLocation for more details
         */
         unsigned long getKeyFromDvaVersionRecordLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getDvaMessageVersionFromDvaMessageKey
         *
         * Returns the version string for the given PaStationDvaMessageKey 
         *
         * @param  stationDvaMessageKey the message key to lookup
         *
         * @return the respective version string
         *
         * @throws CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration 
         *         and/or the refreshZoneIdMaps() method was called with a collection of zoneIds 
         *         that did not include the one of the DataPoints in the collection. 
         */
         std::string getDvaMessageVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * setDvaMessageVersionRecord (passthrough function)
         *
         * @see PaDvaVersionCachedMap::setRecord for more details
         *
         */
        void setDvaMessageVersionRecord(DvaVersionRecord& record, bool writeChangesToDatabase = false);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException);


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Public Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

        /** 
         * refreshKeyToDvaPublicVersionRecordMap
         *
         * @see PaDvaPublicVersionCachedMap::refreshKeyToRecordMap for more details
         *
         */
        void refreshKeyToDvaPublicVersionRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getDvaPublicVersionRecordFromLocationKey
         *
         * @see PaDvaPublicVersionCachedMap::getRecordFromLocation for more details
         */
        DvaPublicVersionRecord getDvaPublicVersionRecordFromLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromDvaPublicVersionRecordLocationKey
         *
         * @see PaDvaPublicVersionCachedMap::getKeyFromLocation for more details
         */
         unsigned long getKeyFromDvaPublicVersionRecordLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getDvaMessagePublicVersionFromDvaMessageKey
         *
         * Returns the version string for the given PaStationDvaMessageKey 
         *
         * @param  stationDvaMessageKey the message key to lookup
         *
         * @return the respective version string
         *
         * @throws CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration 
         *         and/or the refreshZoneIdMaps() method was called with a collection of zoneIds 
         *         that did not include the one of the DataPoints in the collection. 
         */
         std::string getDvaMessagePublicVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * setDvaMessagePublicVersionRecord (passthrough function)
         *
         * @see PaDvaVersionCachedMap::setRecord for more details
         *
         */
        void setDvaMessagePublicVersionRecord(DvaPublicVersionRecord& record, bool writeChangesToDatabase = false);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException);



    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Private Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

        /** 
         * refreshKeyToDvaPrivateVersionRecordMap
         *
         * @see PaDvaPrivateVersionCachedMap::refreshKeyToRecordMap for more details
         *
         */
        void refreshKeyToDvaPrivateVersionRecordMap(unsigned long locationKey = -1) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getDvaVersionRecordFromLocationKey
         *
         * @see PaDvaVersionCachedMap::getRecordFromLocation for more details
         */
        DvaPrivateVersionRecord getDvaPrivateVersionRecordFromLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getKeyFromDvaVersionRecordLocationKey
         *
         * @see PaDvaVersionCachedMap::getKeyFromLocation for more details
         */
         unsigned long getKeyFromDvaPrivateVersionRecordLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getDvaMessageVersionFromDvaMessageKey
         *
         * Returns the version string for the given PaStationDvaMessageKey 
         *
         * @param  stationDvaMessageKey the message key to lookup
         *
         * @return the respective version string
         *
         * @throws CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration 
         *         and/or the refreshZoneIdMaps() method was called with a collection of zoneIds 
         *         that did not include the one of the DataPoints in the collection. 
         */
         std::string getDvaMessagePrivateVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

        /**
         * setDvaMessageVersionRecord (passthrough function)
         *
         * @see PaDvaVersionCachedMap::setRecord for more details
         *
         */
        void setDvaMessagePrivateVersionRecord(DvaPrivateVersionRecord& record, bool writeChangesToDatabase = false);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException);


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      PA Zone and PA Zone Group Record Access
    //
    ///////////////////////////////////////////////////////////////////////////
        /** 
         * refreshKeyToPaZoneRecordMap (passthrough function)
         *
         * @see PaZoneCachedMap::refreshKeyToPaZoneRecordMap for more details
         *
         */
        unsigned long refreshKeyToPaZoneRecordMap(unsigned long locationKey = -1) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
        {
            return m_paZoneCachedMap->refreshKeyToPaZoneRecordMap(locationKey);
        }    
    
        /** 
         * getPaZoneRecordFromKey (passthrough function)
         *
         * @see GenericCachedMap::getCachedDbRecordFromKey for more details
         *
         */
	    PaZoneRecord getPaZoneRecordFromKey(unsigned long key)
           //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return m_paZoneCachedMap->getCachedDbRecordFromKey(key);
        }

        /** 
         * getPaZoneRecordFromKey (passthrough function)
         *
         * @see PaZoneCachedMap::getKeyFromPaZoneIdAndLocation for more details
         *
         */
        unsigned long getKeyFromPaZoneIdAndLocation(unsigned long zoneId, unsigned long locationKey)
           //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return m_paZoneCachedMap->getKeyFromPaZoneIdAndLocation(zoneId, locationKey);
        }


        /** 
         * getPaZoneIdFromKey
         *
         * Returns the PaZoneId associated with the PaZoneRecord key.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  key the primary key of the PaZoneRecord.
         *
         * @return the PaZoneId
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	    unsigned long getPaZoneIdFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return getPaZoneRecordFromKey(key).m_id;
        }


        /** 
         * getPaZoneLocationKeyFromKey
         *
         * Returns the locationKey associated with the PaZoneRecord key.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @param  key the primary key of the PaZoneRecord.
         *
         * @return the PaZoneRecord locationKey
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	    unsigned long getPaZoneLocationKeyFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return getPaZoneRecordFromKey(key).m_locationKey;
        }


        /** 
         * getZoneIdFromEquipmentEntityName
         *
         * @see PaZoneGroupCachedMap::getZoneIdFromEquipmentEntityName
         *
         */        
        PaZoneRecord getRecordFromEquipmentEntityName(const std::string& equipmentEntityName)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return m_paZoneCachedMap->getRecordFromEquipmentEntityName(equipmentEntityName);
        }        


        /** 
         * getPaZoneRecordsFromArea
         *
         * @see PaZoneGroupCachedMap::getPaZoneRecordsFromArea
         *
         */
        PaZoneRecords getPaZoneRecordsFromArea(const std::string& area)
        {
            return m_paZoneCachedMap->getPaZoneRecordsFromArea(area);
        }

        /** 
         * getPaZoneRecordsFromLocationKey
         *
         * @see PaZoneGroupCachedMap::getPaZoneRecordsFromLocationKey
         *
         */
        PaZoneRecords getPaZoneRecordsFromLocationKey(unsigned long locationKey)
        {
            return m_paZoneCachedMap->getPaZoneRecordsFromLocationKey(locationKey);
        }


        /** 
         * refreshKeyToPaZoneGroupRecordMap (passthrough function)
         *
         * @see PaZoneGroupCachedMap::refreshKeyToPaZoneGroupRecordMap
         * 
         */
        void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey = -1) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
        {
            m_paZoneGroupCachedMap->refreshKeyToPaZoneGroupRecordMap(locationKey);
        }
   
    
        /** 
         * getPaZoneGroupRecords (passthrough function)
         *
         * @see GenericCachedMap::getAllRecords
         * 
         */
        PaZoneGroupRecords getPaZoneGroupRecords()
        {
            return m_paZoneGroupCachedMap->getAllRecords();
        }


        /** 
         * getPaZoneGroupRecordFromKey (passthrough function)
         *
         * @see GenericCachedMap::getCachedDbRecordFromKey for more details
         *
         */
	    PaZoneGroupRecord getPaZoneGroupRecordFromKey(unsigned long key)
           //throw(TA_Base_Core::CachedMappingNotFoundException)
        {
            return m_paZoneGroupCachedMap->getCachedDbRecordFromKey(key);
        }

    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Location Access
    //
    ///////////////////////////////////////////////////////////////////////////


        /** 
         * refreshLocationKeyToNameMap
         *
         * Scans the LOCATION table and maps the primary key to the name.
         *
         * @exception DatabaseException 
         *         If there is a problem establishing a connection with the database.
         *         If an error is encountered while retrieving data.
         * @exception DataException      
         *         If the data cannot be converted to the required format.
         *         If the wrong ammount of data is retrieved.     
         *
         */
        void refreshLocationKeyToNameMap() ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /** 
         * getLocationNameFromKey
         *
         * Returns the location name for the specified location key.  This method will call 
         * refreshLocationKeyToNameMap() if the client code has not called it already.
         *
         * @param  key the location primary key
         * @return the respective TA_Base_Bus::DataPoint pointer
         * @exception CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration in the databases.
         *
         */
        std::string getLocationNameFromKey(unsigned long locationKey) ;
            //throw(TA_Base_Core::CachedMappingNotFoundException);


        /** 
         * getLocationKeyFromName
         *
         * Returns the location key for the specified location name.  This method will call 
         * refreshLocationKeyToNameMap() if the client code has not called it already.
         *
         * @param  name the location name
         * @return the location key
         * @exception CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration in the databases.
         *
         */
        unsigned long getLocationKeyFromName(const std::string& locationName) ;
            //throw(TA_Base_Core::CachedMappingNotFoundException);

    ///////////////////////////////////////////////////////////////////////////
    //
    //                Pas Index For Stations Access - List 51
    //
    //                              HARDCODED
    //
    ///////////////////////////////////////////////////////////////////////////


        /** 
         * refreshPasStationIdToLocationNameMap
         *
         * Initialises the PasStationIdToLocationNameMap with hardcoded values
         *
         */
        void refreshKeyToPaStationRecordMap(unsigned long locationKey = -1);

        /** 
         * getPasStationIdFromLocationKey
         *
         * Returns the Pas Station Index for the specified location key.  This method will call 
         * refreshPasStationIdToLocationNameMap() if the client code has not called it already.
         *
         * @param  key the location primary key
         * @return the respective Pas Station Index
         * @exception CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid protocol between 
         *         ISCS and PAS.
         */
        unsigned int getPasStationIdFromLocationKey(unsigned long locationKey);
            //throw(TA_Base_Core::CachedMappingNotFoundException);


        /** 
         * getLocationKeyFromPasStationId
         *
         * Returns the location key for the specified Pas Station Index.  This method will call 
         * refreshPasStationIdToLocationNameMap() if the client code has not called it already.
         *
         * @param  stationId the Pas Station Index
         * @return the respective location key.  Note -1 will be returned if the stationId 
         *         does not have an associated name.  This is to minimise unnecessary exceptions 
         *         throw as PAS data references all 50 stations regardless of whether they are
         *         in existence or not.  
         * @exception CachedMappingNotFoundException 
         *         If the mapping could not be found.  This exception is thrown from a call to
         *         getLocationKeyFromName().  This should not occur if the database has been 
         *         configured correctly.  
         *
         */
        unsigned long getLocationKeyFromPasStationId(unsigned int stationId);
            //throw(TA_Base_Core::CachedMappingNotFoundException);

    ///////////////////////////////////////////////////////////////////////////
    //
    //                            DataPointAccess
    //
    ///////////////////////////////////////////////////////////////////////////



        /** 
         * refreshZoneIdToDataPointMap
         *
         * This method scans through the collection of all DataPoints, determines each TA_Base_Bus::DataPoint's parent entity
         * key (equipment datanode) and utilises the PaZoneRecordMap to determine the PAZoneId.  
         * 
         * If PaZoneRecordMap is refreshed runtime, then this method should be called to ensure that
         * the mapping is consistent.
         *
         * pre: refreshKeyToPaZoneRecordMap() has been called successfully.
         *
         * @throws CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration 
         *         and/or the refreshZoneIdMaps() method was called with a collection of zoneIds 
         *         that did not include the one of the DataPoints in the collection.  
         * @throws InvalidECSConfigurationException 
         *         If a single zoneId is mapped to more than one TA_Base_Bus::DataPoint.  This indicates invalid 
         *         configuration in the databases.
         *
         */
        void refreshZoneIdToDataPointMap(const std::vector<TA_Base_Bus::DataPoint*>& pasDataPoints) ;
            //throw(TA_Base_Core::InvalidPasConfigurationException);

        /** 
         * getStatusDataPointFromZoneId
         *
         * Returns the status datapoint for a given zoneId.
         *
         * @param  zoneId the PA zoneId
         * @return the respective status datapoint.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getStatusDataPointFromZoneId(unsigned long zoneId)
        {
            return getDataPointFromZoneId(zoneId, m_zoneIdToStatusDataPointMap);
        }

        /** 
         * getPriorityDataPointFromZoneId
         *
         * Returns the priority datapoint for a given zoneId.
         *
         * @param  zoneId the PA zoneId
         * @return the respective priority datapoint.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getPriorityDataPointFromZoneId(unsigned long zoneId)   
        {
            return getDataPointFromZoneId(zoneId, m_zoneIdToPriorityDataPointMap);
        }
        /** 
         * getMusicDataPointFromZoneId
         *
         * Returns the music state datapoint for a given zoneId.
         *
         * @param  zoneId the PA zoneId
         * @return the respective music state datapoint.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getMusicDataPointFromZoneId(unsigned long zoneId)
        {
            return getDataPointFromZoneId(zoneId, m_zoneIdToMusicDataPointMap);
        }

        /** 
         * getCyclicDataPointFromZoneId
         *
         * Returns the cyclic state datapoint for a given zoneId.
         *
         * @param  zoneId the PA zoneId
         * @return the respective cyclic state datapoint.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getCyclicDataPointFromZoneId(unsigned long zoneId)
        {
            return getDataPointFromZoneId(zoneId, m_zoneIdToCyclicDataPointMap);
        }

        /** 
         * getLiveDataPointFromZoneId
         *
         * Returns the live state datapoint for a given zoneId.
         *
         * @param  zoneId the PA zoneId
         * @return the respective live state datapoint.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getLiveDataPointFromZoneId(unsigned long zoneId)
        {
            return getDataPointFromZoneId(zoneId, m_zoneIdToLiveDataPointMap);
        }




        void validateAllInitialisedMaps(); //throw(TA_Base_Core::InvalidPasConfigurationException);

	    /**
         * registerPaZoneChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void registerPaZoneChangeObserver(ICachedMapObserver* observer)
        {
            m_paZoneCachedMap->registerObserver(observer);
        }

	    /**
         * registerPaZoneChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void deRegisterPaZoneChangeObserver(ICachedMapObserver* observer)
        {
            m_paZoneCachedMap->deRegisterObserver(observer);
        }


	    /**
         * registerPaZoneGroupChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void registerPaZoneGroupChangeObserver(ICachedMapObserver* observer)
        {
            m_paZoneGroupCachedMap->registerObserver(observer);
        }

	    /**
         * registerPaZoneGroupChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void deRegisterPaZoneGroupChangeObserver(ICachedMapObserver* observer)
        {
            m_paZoneGroupCachedMap->deRegisterObserver(observer);
        }



        /**
         * registerPaStationDvaMessageChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void registerPaStationDvaMessageChangeObserver(ICachedMapObserver* observer);
	    /**
         * registerPaStationDvaMessageChangeObserver (passthrough function)
         *
         * @see GenericCachedMap::registerObserver for more details
         */
        void deRegisterPaStationDvaMessageChangeObserver(ICachedMapObserver* observer);

        /**
         * getPaZoneGroupCachedMap
         *
         * @return the cached map container for PA Zone Groups
         *
         */
        PaZoneGroupCachedMap& getPaZoneGroupCachedMap() { return *m_paZoneGroupCachedMap; }

        /**
         * getPaStationDvaMessageCachedMap
         *
         * @return the cached map container for PA Station Dva Messages
         */
        PaStationDvaMessageCachedMap& getPaStationDvaMessageCachedMap() { return *m_paStationDvaMessageCachedMap; }

    
        /**
         * getPaTrainDvaMessageCachedMap
         *
         * @return the cached map container for PA Train Dva Messages
         */
        PaTrainDvaMessageCachedMap& getPaTrainDvaMessageCachedMap() { return *m_paTrainDvaMessageCachedMap; }

        /**
         * getDvaVersionCachedMap
         *
         * @return the cached map continue for Station DVA Message versions
         *
         */
        PaDvaVersionCachedMap& getDvaVersionCachedMap() { return *m_paDvaVersionCachedMap; }
        PaDvaPublicVersionCachedMap& getDvaPublicVersionCachedMap() { return *m_paDvaPublicVersionCachedMap; }
        PaDvaPrivateVersionCachedMap& getDvaPrivateVersionCachedMap() { return *m_paDvaPrivateVersionCachedMap; }

        /**
         * getPaZoneCachedMap
         * 
         * @return the cached map container for PA zones
         *
         */
        PaZoneCachedMap& getPaZoneCachedMap() { return *m_paZoneCachedMap; }

        /**
         * getPaDvaVersionCachedMap
         * 
         * @return the cached map container for PA Dva version records
         *
         */
        PaDvaVersionCachedMap& getPaDvaVersionCachedMap() { return *m_paDvaVersionCachedMap; }
		PaDvaPublicVersionCachedMap& getPaDvaPublicVersionCachedMap() { return *m_paDvaPublicVersionCachedMap; }
		PaDvaPrivateVersionCachedMap& getPaDvaPrivateVersionCachedMap() { return *m_paDvaPrivateVersionCachedMap; }

        /**
         * getPaAtsTriggeredBroadcastCachedMap
         * 
         * @return the cached map container for PA ATS Triggered broadcast records
         *
         */
        PaAtsTriggeredBroadcastCachedMap& getPaAtsTriggeredBroadcastCachedMap() { return *m_paAtsTriggeredBroadcastCachedMap; }

        /**
         * getPaStationCachedMap
         * 
         * @return the cached map container for PA Station records
         *
         */
        PaStationCachedMap& getPaStationCachedMap() { return *m_paStationCachedMap; }

        /**
         * setOccDepotFlags
         *
         * Flags whether this class is operating from OCC, DEPOT or STATION
         *  Setting both parameters to false indicates STATION
         *
         * @pre OCC and DEPOT are not both set to true
         *
         * @param isOcc true if this is the OCC terminal
         *
         * @param isDepot true if this is the DEPOT terminal
         *     
         */
        void    setOccDepotFlags(bool isOcc, bool isDepot);


        /**
         * getLocationType
         *
         * @return the current location this class is operating from
         *
         * @pre a call to setOccDepotFlags has been made prior
         *
         */
        TA_Base_Core::IPaDvaMessageVersion::ELocation getLocationType();

        /**
         * getDvaMessageVersionFromStationDvaMessage
         *
         * Returns the appropriate version string for the given PaStationDvaMessageKey 
         *
         * @param messageRecord the record we're checking the version for
         *
         * @param versionRecord the record we should use to look up version data (the version
         *          record for the station association with the DVA Message)
         *
         * @pre messageRecord.m_locationKey == versionRecord.m_locationKey
         *
         * exception CachedMappingNotFoundException 
         *         If the mapping could not be found.  This indicates invalid configuration 
         *         and/or the refreshZoneIdMaps() method was called with a collection of zoneIds 
         *         that did not include the one of the DataPoints in the collection. 
         */
        static std::string getDvaMessageVersionFromStationDvaMessage(
                const StationDvaMessageRecord& messageRecord,
                const DvaVersionRecord& versionRecord);

        static std::string getDvaMessagePublicVersionFromStationDvaMessage(
                const StationDvaMessageRecord& messageRecord,
                const DvaPublicVersionRecord& versionRecord);

        static std::string getDvaMessagePrivateVersionFromStationDvaMessage(
                const StationDvaMessageRecord& messageRecord,
                const DvaPrivateVersionRecord& versionRecord);

    protected:

        CachedMaps();
	    virtual ~CachedMaps();

	    void generateRandomData();

        /** 
         * getDataPointFromZoneId
         *
         * Returns the datapoint for a given zoneId from the specified map.
         *
         * @param  zoneId the PA zoneId
         * @param  theMap the ZoneIdToDataPointMap to search from
         * @return the respective datapoint from the map.  Note NULL will be returned if the zoneId 
         *         does not have a configured datapoint.  This is to minimise unnecessary exceptions 
         *         thrown as it is expected that some of the 32 zones are not configured.
         *
         */
        TA_Base_Bus::DataPoint* getDataPointFromZoneId(unsigned long zoneId, ZoneIdToDataPointMap& theMap);

    public:

        // The fuly fledged datapoint name will be determined by appending the 
        // strings below with the PA zone equipment datanode entity names given
        // in the PA_ZONE table.

        static const std::string            PA_ZONE_STATUS_TOKEN;
        static const std::string            PA_ZONE_PRIORITY_TOKEN;
        static const std::string            PA_ZONE_LIVE_TOKEN;
        static const std::string            PA_ZONE_CYCLIC_TOKEN;
        static const std::string            PA_ZONE_MUSIC_TOKEN;

    protected:


   	    static CachedMaps*                                          m_me;

        // Synchronisation locks for each of the maps
        static TA_Base_Core::NonReEntrantThreadLockable                  m_lockForSingleton;

        TA_Base_Core::NonReEntrantThreadLockable                         m_lockForLocationKeyToNameMap;
        TA_Base_Core::NonReEntrantThreadLockable                         m_lockForZoneIdToDataPointMaps;
    
    
        LocationKeyToNameMap                                        m_locationKeyToNameMap;

        ZoneIdToDataPointMap                                        m_zoneIdToStatusDataPointMap;
        ZoneIdToDataPointMap                                        m_zoneIdToPriorityDataPointMap;
        ZoneIdToDataPointMap                                        m_zoneIdToLiveDataPointMap;
        ZoneIdToDataPointMap                                        m_zoneIdToCyclicDataPointMap;
        ZoneIdToDataPointMap                                        m_zoneIdToMusicDataPointMap;

        // Flags for each of the maps to ensure that they are initialised before accessing.
        bool                                                        m_isLocationKeyToNameMapInitialised;
        bool                                                        m_isZoneIdToDataPointMapsInitialised;


    private:

        // Make these pointers to headers don't need to be included (in CachedMaps header)
        PaZoneCachedMap*                                            m_paZoneCachedMap;
        PaZoneGroupCachedMap*                                       m_paZoneGroupCachedMap;
        PaStationDvaMessageCachedMap*                               m_paStationDvaMessageCachedMap;    
        PaTrainDvaMessageCachedMap*                                 m_paTrainDvaMessageCachedMap;
        PaDvaVersionCachedMap*                                      m_paDvaVersionCachedMap;
        PaAtsTriggeredBroadcastCachedMap*                           m_paAtsTriggeredBroadcastCachedMap;
        PaStationCachedMap*                                         m_paStationCachedMap;
		PaDvaPublicVersionCachedMap*                                m_paDvaPublicVersionCachedMap;
        PaDvaPrivateVersionCachedMap*                               m_paDvaPrivateVersionCachedMap;

        TA_Base_Core::PrimitiveWrapper<TA_Base_Core::IPaDvaMessageVersion::ELocation> m_locationType;

        /// Simple inner class to define required interface for the zone group cached map    
        class LocationNameHelper : public ILocationNameHelper
        {
            virtual std::string getLocationNameFromKey(unsigned long locationKey)
            {
                return CachedMaps::getInstance()->getLocationNameFromKey(locationKey);
            }
        };
        static LocationNameHelper s_locationNameHelper;    
    };

} // end namespace TA_IRS_Bus
    
    
#endif // !defined(PA_CACHED_MAPS_INCLUDED)
