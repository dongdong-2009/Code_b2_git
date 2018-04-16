/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ATS PSD Entity Map information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(AtsPowerEntityMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define AtsPowerEntityMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_IRS_Core
{
    class IConfigAtsPowerData;
}


namespace TA_IRS_App
{
    class AtsPowerEntityMapDatabaseAccessor : public TA_Base_App::IDatabaseAccessor
    {

    public:

		typedef std::map<unsigned long, std::string>					DataPointKeyToNameMap;
        typedef DataPointKeyToNameMap::iterator							DataPointKeyToNameMapIt;

        struct AtsPowerEntityMapRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it

			unsigned long key;			// The primary key
			CString mssZone;			// The MSS Zone of the station
            CString location;			// The Location
			CString subsection;			// The number of the electrical subsection
            CString dpName;				// The name of the DataPoint
        };


        /**
         * Constructor
         */
        AtsPowerEntityMapDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~AtsPowerEntityMapDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the ATS PSD Entity Map records from the database and store them in m_AtsPowerEntityMaps.
         *
         * @exception DataException - This will be thrown if there is an error with the ATS PSD Entity Map
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by ATS PSD Entity Maps and will just assert
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
         * @return map<unsigned long, AtsPowerEntityMapRecord> - This is all the ATS PSD Entity Maps keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,AtsPowerEntityMapRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return AtsPowerEntityMapRecord - This is all the ATS PSD Entity Map details
         */
        AtsPowerEntityMapRecord getItemDetails(unsigned long identifier);
 
        
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
         * getAllPsdDataPoints
         *
         * This returns all PSD datapoints and keys to the caller.  Ensure that loadItems() is called first
         * otherwise no data will be returned.
         *
         * @return map<unsigned long, string> - This returns all the datapoint entity keys and names. This is all
         *                                      information needed to update data and to display to the user
         */
        DataPointKeyToNameMap getAllPsdDataPoints()
		{
            return m_dataPointNameMap;
        }

        std::string getDataPointName(unsigned long dataPointKey);
		std::string getDataPointLocation(unsigned long dataPointKey);
		std::string getDataPointMssZone(unsigned long dataPointKey);
        unsigned long getDataPointKey(const std::string& dataPointName);

    protected:
		AtsPowerEntityMapRecord formatItemDetails(TA_IRS_Core::IConfigAtsPowerData* config);
        void refreshDataPointMap();
   
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AtsPowerEntityMapDatabaseAccessor( const AtsPowerEntityMapDatabaseAccessor& theAtsPowerEntityMapDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AtsPowerEntityMapDatabaseAccessor& operator=(const AtsPowerEntityMapDatabaseAccessor& pm);

        typedef std::map<unsigned long, TA_IRS_Core::IConfigAtsPowerData*>   ConfigItems;
        typedef ConfigItems::iterator                               ConfigItemsIt;


        ConfigItems					m_configItems;      // This is all the ATS PSD Entity Maps loaded from the database
        DataPointKeyToNameMap		m_dataPointNameMap;

        bool m_isLoaded;                // This indicates if we have loaded the ATS PSD Entity Maps before or not.
                                        // If we have never loaded the ATS PSD Entity Maps then when we are told
                                        // to refresh we shouldn't load them.

    };
}

#endif // !defined(AtsPowerEntityMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
