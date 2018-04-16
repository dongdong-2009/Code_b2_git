/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_zone_group/src/PAZoneGroupDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class uses the Data Access Interface to retrieve PA Zone Group information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(PaZoneGroupDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define PaZoneGroupDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>


#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ItemKeyToNameMap.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_Base_Core
{
    class IConfigPaZoneGroup;
}

namespace TA_IRS_App
{
    class PaZoneGroupDatabaseAccessor 
    : 
    public TA_Base_App::IDatabaseAccessor
    {

    public:

        typedef std::map<unsigned long, std::string>    LocationKeyToNameMap;
        typedef LocationKeyToNameMap::iterator          LocationKeyToNameMapIt;


        struct PaZoneGroupRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it
            CString key;                
            CString location;          
            CString label;         
            CString isEventGroup;                               
        };

        /**
         * Constructor
         */
        PaZoneGroupDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~PaZoneGroupDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the PA Zone Group records from the database and store them in m_paZoneGroups.
         *
         * @exception DataException - This will be thrown if there is an error with the PA Zone Group
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by PA Zone Groups and will just assert
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
         * @return map<unsigned long, PaZoneGroupRecord> - This is all the PA Zone Groups keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,PaZoneGroupRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return PaZoneGroupRecord - This is all the PA Zone Group details
         */
        PaZoneGroupRecord getItemDetails(unsigned long identifier);
 
        
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
        LocationKeyToNameMap getAllLocations()
        {
            return m_locationNameCache.getInternalMap();
        }
        
        /**
         * getAllZonesForLocation
         *
         * @return map of zones for the input location
         * @param locationKey the location filter - the OCC location means all zones are returned
         */
        TA_IRS_Bus::PaZoneRecords getAllZonesForLocation(unsigned long locationKey);
    
        /**
         * getZoneFromKey
         *
         * @return the zone defined by the input key
         * @param zoneKey the primary key of the zone we're fetching the information for
         * @exception CachedMappingNotFoundException if the key was unrecognized
         */
        TA_IRS_Bus::PaZoneRecord getZoneFromKey(unsigned long zoneKey);

        /**
         * getLocationName
         * 
         * @return a string representation of the location defined by 'locationKey'
         * @param locationKey the primary key of the location
         */
        std::string getLocationName(unsigned long locationKey);
        
        /**
         * getLocationKey
         * 
         * Provides the inverse operation to getLocationName
         * @see getLocationName
         */
        unsigned long getLocationKey(const std::string& locationName);
        
        /**
         * isOccLocationKey
         *
         * @return true if the input 'locationKey' defined the OCC location
         *
         */
        bool isOccLocationKey(unsigned long locationKey);

        /**
         * @return the location key of the depot station
         */
        //unsigned long getDepotLocationKey();

    protected:

        void refreshCachedMaps();
   
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneGroupDatabaseAccessor( const PaZoneGroupDatabaseAccessor& thePaZoneGroupDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneGroupDatabaseAccessor& operator=(const PaZoneGroupDatabaseAccessor& pm);

        /**
         * getRecordFrom
         *
         * Converts data from IConfigPaZoneGroup into PaZoneGroupRecord structure
         */
        PaZoneGroupRecord getRecordFrom(TA_Base_Core::IConfigPaZoneGroup& item);

        typedef std::map<unsigned long, TA_Base_Core::IConfigPaZoneGroup*>    ConfigItems;
        typedef ConfigItems::iterator                                       ConfigItemsIt;


        ConfigItems             m_configItems;      // This is all the PA Zone Groups loaded from the database
        

        bool m_isLoaded;                // This indicates if we have loaded the PA Zone Groups before or not.
                                        // If we have never loaded the PA Zone Groups then when we are told
                                        // to refresh we shouldn't load them.

        ItemKeyToNameMap<TA_Base_Core::ILocation> m_locationNameCache;

		std::map<unsigned long, TA_Base_Core::ILocation::ELocationType> m_locationTypeMap;
        // Store a cached copy for efficiency
    };
}

#endif // !defined(PaZoneGroupDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
