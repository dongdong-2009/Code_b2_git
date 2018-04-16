/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class uses the Data Access Interface to retrieve Ats Branch Track information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(AtsBranchTrackDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define AtsBranchTrackDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

// Forward declarations
namespace TA_IRS_Core
{
    class IConfigAtsBranchTrack;
    class IConfigItem;
}


namespace TA_IRS_App
{
    class AtsBranchTrackDatabaseAccessor : public TA_Base_App::IDatabaseAccessor
    {

    public:

        struct AtsBranchTrackRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it

			unsigned long key;			// The key as a number
            CString atsBranchId;        // The Ats Branch ID and primary key
            CString track;              // The Track
        };

        /**
         * Constructor
         */
        AtsBranchTrackDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~AtsBranchTrackDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the Ats Branch Track records from the database and store them in m_AtsBranchTracks.
         *
         * @exception DataException - This will be thrown if there is an error with the Ats Branch Track
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by Ats Branch Tracks and will just assert
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
         * @return map<unsigned long, AtsBranchTrackRecord> - This is all the Ats Branch Tracks keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,AtsBranchTrackRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return AtsBranchTrackRecord - This is all the Ats Branch Track details
         */
        AtsBranchTrackRecord getItemDetails(unsigned long identifier);
 
        
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
        AtsBranchTrackDatabaseAccessor( const AtsBranchTrackDatabaseAccessor& theAtsBranchTrackDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AtsBranchTrackDatabaseAccessor& operator=(const AtsBranchTrackDatabaseAccessor& pm);

        typedef std::map<unsigned long, TA_IRS_Core::IConfigAtsBranchTrack*>   ConfigItems;
        typedef ConfigItems::iterator                               ConfigItemsIt;


        ConfigItems             m_configItems;      // This is all the Ats Branch Tracks loaded from the database
        
        bool m_isLoaded;                // This indicates if we have loaded the Ats Branch Tracks before or not.
                                        // If we have never loaded the Ats Branch Tracks then when we are told
                                        // to refresh we shouldn't load them.

    };
}

#endif // !defined(AtsBranchTrackDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
