/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve PA Station DVA Message information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(PaStationDvaMessageDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define PaStationDvaMessageDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ItemKeyToNameMap.h"
#include "bus/pa/cachedmaps/src/ILocationNameHelper.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigPaTisMessageMap;
}

namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_IRS_App
{
    class PaStationDvaMessageDatabaseAccessor 
    : 
    public TA_Base_App::IDatabaseAccessor, 
    public TA_IRS_Bus::ILocationNameHelper
    {

    public:

        typedef std::map<unsigned long, std::string>    LocationKeyToNameMap;
        typedef LocationKeyToNameMap::iterator          LocationKeyToNameMapIt;
        
        struct PaStationDvaMessageRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it
            CString key;    
                        
            CString location;
            CString id;          
            CString label;         
            CString type;
            CString tisMessageTag;
            CString tisLibrarySection;
            CString tisLibraryVersion;
            CString tisMessageDescription;
        };

        /**
         * Constructor
         */
        PaStationDvaMessageDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~PaStationDvaMessageDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the PA Station DVA records from the database and stores them locally.
         *
         * @exception DataException - This will be thrown if there is an error with the PA Station DVA Message
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by PA Station DVA Messages and will just assert
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
         * @return map<unsigned long, ConfigPaDvaMessage> - This is all the PA Station DVA Messages keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,PaStationDvaMessageRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return ConfigPaDvaMessage - This is all the PA Station DVA Message details
         */
        PaStationDvaMessageRecord getItemDetails(unsigned long identifier);
 
        
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
        //bool isOccLocationKey(unsigned long locationKey);
        
        /**
         * getLocationNameFromKey (interface implementation for ILocationNameHelper)
         * Performs function passthrough
         */
        virtual std::string getLocationNameFromKey(unsigned long locationKey) { return getLocationName(locationKey); }
        
        /**
         * getTypeString
         *
         * @return the extended description of a station dva message type
         *
         * @param type the string defining the type of message, as taken from IPaDvaMessage::getType
         */
        static CString getTypeString(const std::string type);        


        /**
         * updateConfigItemsTisMessage
         *
         * Updates the TIS message links (affecting 'configItem') to point to the TIS message
         *  defined by tisKey
         *
         * @return true if one or more of the parameters of configItem were set
         *
         * @param configItem the configuration item which is to be updated to point to 'tisKey'
         *
         * @param tisKey primary key of message to set to - this may be to 
         *      TA_Base_Core::DatabaseKey::getInvalidKey() to 'unset' the TIS message
         *
         * @exception TransactiveException if the input key is unrecognized, no change made
         *
         */
        bool updateConfigItemsTisMessage(TA_Base_Core::IConfigPaTisMessageMap& configItem, unsigned long tisKey);

        /**
         * getAllTisMessage
         *
         * @return an array of all known TIS Messages
         *
         */
        std::vector<TA_Base_Core::PaDvaMessageAccessFactory::TisMessage> getAllTisMessages();

        /**
         * getTisMessage
         *
         * @return the tis message associated with the input config item
         *
         * @param configItem the configuration item defining which tis message we want to find
         *
         * @exception TransactiveException if the target message couldn't be found
         */
        TA_Base_Core::PaDvaMessageAccessFactory::TisMessage getTisMessage(TA_Base_Core::IConfigPaTisMessageMap& configItem);
        TA_Base_Core::PaDvaMessageAccessFactory::TisMessage getTisMessage(unsigned long key) const;

    protected:

        void refreshCachedMaps();
   
    private:
  
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaStationDvaMessageDatabaseAccessor( const PaStationDvaMessageDatabaseAccessor& thePaStationDvaMessageDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaStationDvaMessageDatabaseAccessor& operator=(const PaStationDvaMessageDatabaseAccessor& pm);

        /**
         * getRecordFrom
         *
         * Converts data from IConfigPaDvaMessage into ConfigPaDvaMessage structure
         */
        PaStationDvaMessageRecord getRecordFrom(TA_Base_Core::IConfigPaTisMessageMap& item);
 

        typedef std::map<unsigned long, TA_Base_Core::IConfigPaTisMessageMap*>    ConfigItems;
        typedef ConfigItems::iterator                                       ConfigItemsIt;

        ConfigItems             m_configItems;      // This is all the PA Station DVA Messages loaded from the database
        

        typedef std::map<unsigned long, TA_Base_Core::PaDvaMessageAccessFactory::TisMessage> TisMessages;
        TisMessages m_tisMessages;

        bool m_isLoaded;                // This indicates if we have loaded the PA Station DVA Messages before or not.
                                        // If we have never loaded the PA Station DVA Messages then when we are told
                                        // to refresh we shouldn't load them.

        ItemKeyToNameMap<TA_Base_Core::ILocation> m_locationNameCache;
    };
}

#endif // !defined(PaStationDvaMessageDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
