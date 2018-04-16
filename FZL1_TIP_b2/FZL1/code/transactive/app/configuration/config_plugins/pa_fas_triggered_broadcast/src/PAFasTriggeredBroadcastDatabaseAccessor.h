/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/3002/transactive/app/configuration/config_plugins/pa_fas_triggered_broadcast/src/PAFasTriggeredBroadcastDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/27 14:02:43 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This class uses the Data Access Interface to retrieve PA FAS Triggered Broadcast information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(PaFasTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define PaFasTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ItemKeyToNameMap.h"
#include "bus/pa/cachedmaps/src/ILocationNameHelper.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "core/data_access_interface/src/DatabaseKey.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_Base_Core
{
    class IConfigPaFasTriggeredBroadcast;
}

namespace TA_IRS_Bus
{
    class PaStationDvaMessageCachedMap;
    class PaZoneGroupCachedMap;
}

namespace TA_IRS_App
{
    class PaFasTriggeredBroadcastDatabaseAccessor 
    : 
    public TA_Base_App::IDatabaseAccessor, 
    public TA_IRS_Bus::ILocationNameHelper
    {

    public:
        
        enum EDataSet
        {
            DATA_ZONE_GROUP,
            DATA_DVA_MESSAGE,
            DATA_FAS_ALARM_ENTITY            
        };

        typedef std::map<unsigned long, std::string>    LocationKeyToNameMap;
        typedef LocationKeyToNameMap::iterator          LocationKeyToNameMapIt;
        
        struct CfgPaFasTriggeredBroadcastRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it
            CString key;    
            CString fasAlarmEntityName;                            
            CString location;
            CString cyclicTimes;
            CString zoneGroupName;
            CString dvaMessage;
            CString isEnabled;  
        };
        
        /**
         * Constructor
         */
        PaFasTriggeredBroadcastDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~PaFasTriggeredBroadcastDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the PA FAS Triggered Broadcast records from the database and stores them locally.
         *
         * @exception DataException - This will be thrown if there is an error with the PA FAS Triggered Broadcast
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by PA FAS Triggered Broadcasts and will just assert
         */
        std::multimap<CString,unsigned long> getItemNames(CProgressCtrl& progress);
        

        /**
         * getAllItemDetails
         *
         * This retrieves the items' details in a display format.
         * Precondition: loadItems() must be called before this method
         * 
         * @param CProgressCtrl - This is used to show how long the loading is taking
         *
         * @return map<unsigned long, ConfigPaDvaMessage> - This is all the PA FAS Triggered Broadcasts keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long, CfgPaFasTriggeredBroadcastRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return ConfigPaDvaMessage - This is all the PA FAS Triggered Broadcast details
         */
        CfgPaFasTriggeredBroadcastRecord getItemDetails(unsigned long identifier);
 
        
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
            return m_mapLocationKeyToDisplayName;
        }
                
        /**
         * getLocationName
         * 
         * @return a string representation of the location defined by 'locationKey'
         * @param locationKey the primary key of the location
         */
        std::string getLocationName(unsigned long locationKey) const;
                
        /**
         * getLocDisplayName
         * 
         * @return a string representation of the location defined by 'locationKey'
         * @param locationKey the primary key of the location
         */
        std::string getLocDisplayName(unsigned long locationKey) const;

        /**
         * getFasAlarmEntityName
         *
         * @param key the primary key of the record in database
         *
         * @param locationKeyOut(optional) returns the location key of the input record
         *
         * @return a name identifying the record
         *
         */
        CString getFasAlarmEntityName(unsigned long key, unsigned long* locationKeyOut = 0);

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
         * getLocationNameFromKey (interface implementation for ILocationNameHelper)
         * Performs function passthrough
         */
        virtual std::string getLocationNameFromKey(unsigned long locationKey) { return getLocationName(locationKey); }

        /**
         * getLocationOrderIdFromKey (interface implementation for ILocationNameHelper)
         * Performs function passthrough
         */
		virtual unsigned long getLocationOrderIdFromKey( unsigned long ulLocationKey ) {return 0;} // TODO
        /**
         * getLocationNameFromKey
         * Performs function passthrough
         */
        virtual std::string getLocDisplayNameFromKey(unsigned long locationKey) { return getLocDisplayName(locationKey); }

        /**
         * getRecordFrom
         *
         * Converts data from IConfigPaFasTriggeredBroadcast into ConfigPaDvaMessage structure
         */
        CfgPaFasTriggeredBroadcastRecord getRecordFrom(TA_Base_Core::IConfigPaFasTriggeredBroadcast& item);
        
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
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getDvaMessageDataForSelection(unsigned long locationKeyFilter) const;
        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getZoneGroupDataForSelection(unsigned long locationKeyFilter) const;
        /**
         * Helper function, @see getDataForSelection for more details
         */
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getFasAlarmEntityDataForSelection(unsigned long locationKeyFilter) const;

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaFasTriggeredBroadcastDatabaseAccessor( const PaFasTriggeredBroadcastDatabaseAccessor& thePaFasTriggeredBroadcastDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaFasTriggeredBroadcastDatabaseAccessor& operator=(const PaFasTriggeredBroadcastDatabaseAccessor& pm);
 

        typedef std::map<unsigned long, TA_Base_Core::IConfigPaFasTriggeredBroadcast*>    ConfigItems;
        typedef ConfigItems::iterator                                       ConfigItemsIt;

        ConfigItems             m_configItems;      // This is all the PA FAS Triggered Broadcasts loaded from the database
        
        bool m_isLoaded;                // This indicates if we have loaded the PA FAS Triggered Broadcasts before or not.
                                        // If we have never loaded the PA FAS Triggered Broadcasts then when we are told
                                        // to refresh we shouldn't load them.

        ItemKeyToNameMap<TA_Base_Core::ILocation> m_locationNameCache;
        std::map<unsigned long, std::string> m_mapLocationKeyToDisplayName;

        TA_IRS_Bus::PaStationDvaMessageCachedMap*   m_stationDvaMessageCachedMap;
        TA_IRS_Bus::PaZoneGroupCachedMap*           m_zoneGroupCachedMap;

        typedef std::map<unsigned long, TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::FasAlarmEntity*> FasAlarmEntityMap;
        FasAlarmEntityMap m_fasAlarmEntities;
    };
}

#endif // !defined(PaFasTriggeredBroadcastDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
