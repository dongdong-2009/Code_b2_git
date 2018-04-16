/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class uses the Data Access Interface to retrieve PA ATS Triggered Broadcast information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(PaAtsTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define PaAtsTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/pa_4669/src/PaAtsTriggeredBroadcastAccessFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ItemKeyToNameMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "core/data_access_interface/src/DatabaseKey.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigItem;
    class IConfigPaAtsTriggeredBroadcast;
}

namespace TA_IRS_Bus
{
    class PaStationDvaMessageCachedMap;
    class PaZoneGroupCachedMap;
    class PaDvaVersionCachedMap;
}

namespace TA_IRS_App
{
    class PaAtsTriggeredBroadcastDatabaseAccessor 
    : 
    public TA_Base_App::IDatabaseAccessor
    {

    public:
        
        enum EDataSet
        {
            DATA_ZONE_GROUP,
            DATA_PLATFORM_ID,
            DATA_DVA_MESSAGE,
            DATA_EVENT_TYPE            
        };

        typedef std::map<unsigned long, std::string>    LocationKeyToNameMap;
        typedef LocationKeyToNameMap::iterator          LocationKeyToNameMapIt;
        
        struct CfgPaAtsTriggeredBroadcastRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it
            CString key;    
            
            CString eventType;                            
            CString location;
            CString destLocation;
            CString platformId;
            CString isLastTrain;
            CString zoneGroupName;
            CString dvaMessage;
            CString isEnabled;
            CString dvaMessageVersion;      
        };
        
        /**
         * Constructor
         */
        PaAtsTriggeredBroadcastDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~PaAtsTriggeredBroadcastDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the PA ATS Triggered Broadcast records from the database and stores them locally.
         *
         * @exception DataException - This will be thrown if there is an error with the PA ATS Triggered Broadcast
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by PA ATS Triggered Broadcasts and will just assert
         */
        std::map<CString,unsigned long> getItemNames(CProgressCtrl& progress);
        

        /**
         * getAllItemDetails
         *
         * This retrieves the items' details in a display format.
         * Precondition: loadItems() must be called before this method
         * 
         * @param CProgressCtrl - This is used to show how long the loading is taking
         *
         * @return map<unsigned long, ConfigPaDvaMessage> - This is all the PA ATS Triggered Broadcasts keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long, CfgPaAtsTriggeredBroadcastRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return ConfigPaDvaMessage - This is all the PA ATS Triggered Broadcast details
         */
        CfgPaAtsTriggeredBroadcastRecord getItemDetails(unsigned long identifier);
 
        
        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its unique identifier..
         * Precondition: loadItems() must be called before this method
         *
         * @param unsigned long - The identifier of the item to retrieve
         *
         * @return IConfigItem* - The database object for this item. This will be NULL if one
         *                            is not found.
         */
        virtual TA_Base_Core::IConfigItem* getItem(unsigned long identifier);


        /**
         * deleteItem
         *
         * This will delete the specified item from the database and it will then delete the
         * IConfigItem object held.
         *
         * @param unsigned long - This is the unique identifier for the item to delete
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is an error
         *                              deleting the item
         */
        virtual void deleteItem(unsigned long id);


        /**
         * newItem
         *
         * This creates a new item. This item will be added to the internal list
         * of current items but will not yet be applied to the database.
         *
         * @return IConfigItem* - The newly created item ready for its data to be populated
         */
        virtual TA_Base_Core::IConfigItem* newItem();


        /**
         * copyItem
         *
         * This creates a new item with the same data in it as the item passed in.
         * This item will be added to the internal list of current items but will not yet be
         * applied to the database.
         *
         * @param unsigned long - The unique identifier of the item to copy
         * 
         * @return IConfigItem* - The newly created item ready for its data to be populated
         *
         * @exception DatabaseException - This is thrown if data could not be retrieved for the item
         *                                that is being copied.
         */
        virtual TA_Base_Core::IConfigItem* copyItem(const unsigned long idOfItemToCopy);

        
        /**
         * areCurrentChangesPending
         *
         * This is called to determine if there are any current changes that have not been applied.
         * If this returns true then it will populate the vector of strings passed in with all items
         * not applied.
         * 
         * @param vector<string>& - This is an OUT parameter and should be passed in empty. It will
         *                          be populated with names for all data not yet applied
         * 
         * @return bool - This will return true if there are items that have had changes which have  not
         *                been applied.
         */
         bool areCurrentChangesPending(std::vector<std::string>& itemsNotApplied);

        
         /**
          * invalidate
          *
          * This will set m_isLoaded to false indicating that next time load is called all data should
          * be refreshed from the database.
          */
         void invalidateData()
         {
             m_isLoaded = false;
         }


        /**
         * getAllLocations
         *
         * This returns all location name and keys to the caller.  Ensure that loadItems() is called first
         * otherwise no data will be returned.
         *
         * @return map<unsigned long, string> - This returns all the location keys and names. This is all
         *                                      information needed to update data and to display to the user
         */
        LocationKeyToNameMap getAllLocations() const
        {
            return m_locationNameCache.getInternalMap();
        }
                
        /**
         * getLocationName
         * 
         * @return a string representation of the location defined by 'locationKey'
         * @param locationKey the primary key of the location
         */
        std::string getLocationName(unsigned long locationKey) const;        

        /**
         * getAtsEventTypeName
         *
         * @param key the primary key of the record in database
         *
         * @return a name identifying the record
         *
         */
        CString getAtsEventTypeName(unsigned long key);

        /**
         * getPlatformName
         *
         * @param key the primary key of the record in database
         *
         * @param locationKeyOut(optional) returns the location key of the input record
         *
         * @return a name identifying the record
         *
         */
        CString getPlatformName(unsigned long key, unsigned long* locationKeyOut = 0);

        /**
         * getZoneGroupName
         *
         * @param key the primary key of the record in database
         *
         * @param locationKeyOut(optional) returns the location key of the input record
         *
         * @return a name identifying the record
         *
         */
        CString getZoneGroupName(unsigned long key, unsigned long* locationKeyOut = 0);

        /**
         * getDvaMessageName
         *
         * @param key the primary key of the record in database
         *
         * @param locationKeyOut(optional) returns the location key of the input record
         *
         * @return a name identifying the record
         *
         */
        CString getDvaMessageName(unsigned long key, unsigned long* locationKeyOut = 0);

        /**
         * getLocationKey
         * 
         * Provides the inverse operation to getLocationName
         * @see getLocationName
         */
        unsigned long getLocationKey(const std::string& locationName);

        /**
         * getRecordFrom
         *
         * Converts data from IConfigPaAtsTriggeredBroadcast into ConfigPaDvaMessage structure
         */
        CfgPaAtsTriggeredBroadcastRecord getRecordFrom(TA_Base_Core::IConfigPaAtsTriggeredBroadcast& item);
        
        /**
         * getDataForSelection
         *
         * @return an array of dialog data suitable for use in a selection dialog, the
         *      data is taken from the relevant source as defined by 'dataSet'
         *      and will be synchronized with the header structure returned by
         *      getDataSetHeaderProperties
         *
         * @param dataSet defines the set of data we're operating for
         *
         * @param locationFilter[optional] set to invalid key to return all data, 
         *          otherwise filter by this location key
         *
         * @see getDataSetHeaderProperties
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
        getDataForSelection(EDataSet dataSet, unsigned long locationKeyFilter = TA_Base_Core::DatabaseKey::getInvalidKey()) const;
    
        /**
         * getDataSetHeaderProperties
         *
         * @return a header properties structure suitable for use in conjunction with information
         *      returned by getDataForSelection
         *
         * @param dataSet defines the type of data we're wanting the header properties for
         *
         * @see getDataForSelection         
         */
        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> getDataSetHeaderProperties(EDataSet dataSet);

        /**
         * getLatestVersionForMessage
         *
         * Looks at the local cached map of the DVA Set Version records, and determines
         *  (from this) the most up to date version string for the message defined in argument
         *
         * @param stationDvaMessagePrimaryKey the primary key of the Station DVA Message we're
         *          obtaining the version information for
         *
         * @return string representation of the version for this DVA message
         *
         */
        std::string getLatestVersionForMessage(unsigned long stationDvaMessagePrimaryKey);

        std::string getDvaMsgTypeString(const unsigned short type) const;

    protected:
        
        /**
         * refreshCachedMaps
         *
         * Refreshes local cache data (called once off)
         *
         */
        void refreshCachedMaps();
   
    private:

        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getEventTypeDataForSelection() const;
        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getDvaMessageDataForSelection(unsigned long locationKeyFilter) const;
        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getPlatformIdDataForSelection(unsigned long locationKeyFilter) const;       
        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getZoneGroupDataForSelection(unsigned long locationKeyFilter) const;

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaAtsTriggeredBroadcastDatabaseAccessor( const PaAtsTriggeredBroadcastDatabaseAccessor& thePaAtsTriggeredBroadcastDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaAtsTriggeredBroadcastDatabaseAccessor& operator=(const PaAtsTriggeredBroadcastDatabaseAccessor& pm);
 

        typedef std::map<unsigned long, TA_Base_Core::IConfigPaAtsTriggeredBroadcast*>    ConfigItems;
        typedef ConfigItems::iterator                                       ConfigItemsIt;

        ConfigItems             m_configItems;      // This is all the PA ATS Triggered Broadcasts loaded from the database
        
        bool m_isLoaded;                // This indicates if we have loaded the PA ATS Triggered Broadcasts before or not.
                                        // If we have never loaded the PA ATS Triggered Broadcasts then when we are told
                                        // to refresh we shouldn't load them.

        ItemKeyToNameMap<TA_Base_Core::ILocation> m_locationNameCache;

        typedef std::map<TA_IRS_Bus::IAtsPasCorbaDef::EEventType, std::string> EventTypeMap;
        EventTypeMap m_eventTypes;

        TA_IRS_Bus::PaStationDvaMessageCachedMap*   m_stationDvaMessageCachedMap;
        TA_IRS_Bus::PaZoneGroupCachedMap*           m_zoneGroupCachedMap;

        TA_IRS_Bus::PaDvaVersionCachedMap*          m_dvaVersionCachedMap;

        typedef std::map<unsigned long, TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::PlatformId> PlatformMap;
        PlatformMap m_platformIds;
    };
}

#endif // !defined(PaAtsTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
