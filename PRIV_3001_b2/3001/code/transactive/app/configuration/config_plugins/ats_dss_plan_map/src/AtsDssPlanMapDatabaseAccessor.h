/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class uses the Data Access Interface to retrieve DSS Plan Map information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(AtsDssPlanMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define AtsDssPlanMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

// Forward declarations
namespace TA_IRS_Core
{
    class IConfigAtsDssPlanMap;
}

namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_IRS_App
{
    class AtsDssPlanMapDatabaseAccessor : public TA_Base_App::IDatabaseAccessor
    {

    public:

        struct AtsDssPlanMap
        {
            CString status;				// This will be the current applied status of the item (ie new, modified etc)
										// It will be set elsewhere so we don't worry about it
            int		dssEvent;			// The DSS Event number
			CString	dssEventCString;
            CString plan;				// The plan name
        };


        /**
         * Constructor
         */
        AtsDssPlanMapDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~AtsDssPlanMapDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the DSS Plan Maps from the database and store them in m_atsDssPlanMaps.
         *
         * @exception DataException - This will be thrown if there is an error with the DSS Plan Map
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by DSS Plan Maps and will just assert
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
         * @return map<unsigned long, AtsDssPlanMap> - This is all the DSS Plan Maps keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,AtsDssPlanMap> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return AtsDssPlanMap - This is all the DSS Plan Map details
         */
        AtsDssPlanMap getItemDetails(unsigned long identifier);
 
        
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

    
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AtsDssPlanMapDatabaseAccessor( const AtsDssPlanMapDatabaseAccessor& theAtsDssPlanMapDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AtsDssPlanMapDatabaseAccessor& operator=(const AtsDssPlanMapDatabaseAccessor& pm);


        // This is a map of IConfigAtsDssPlanMap* objects stored by their key (unsigned long)
        typedef std::map<unsigned long, TA_IRS_Core::IConfigAtsDssPlanMap*> LoadedAtsDssPlanMaps;

        LoadedAtsDssPlanMaps m_atsDssPlanMaps;   // This is all the DSS Plan Maps loaded from the database

        bool m_isLoaded;          // This indicates if we have loaded the DSS Plan Maps before or not.
                                  // If we have never loaded the DSS Plan Maps then when we are told
                                  // to refresh we shouldn't load them.

    };
}

#endif // !defined(AtsDssPlanMapDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
