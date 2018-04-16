/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/trainradiogroupdatabaseaccessor.h $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This class uses the Data Access Interface to retrieve system controller information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */
#if !defined(TrainRadioGroupDatabaseAccessor_H)
#define TrainRadioGroupDatabaseAccessor_H


#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"


namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:25 PM
     */

    class TrainRadioGroupDatabaseAccessor : public TA_Base_App::IDatabaseAccessor
    {

    public:


        /**
         * Constructor
         */
        TrainRadioGroupDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupDatabaseAccessor();


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
        bool areCurrentChangesPending( std::vector<std::string>& itemsNotApplied );


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
        virtual TA_Base_Core::IConfigItem* copyItem( const unsigned long idOfItemToCopy );


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
        virtual void deleteItem( unsigned long id );


        /**
         * getAllLocations
         *
         * This returns all locations and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the group keys and names.
         */
        std::map<unsigned long, std::string> getAllLocations();


        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its key.
         * Precondition: loadItems() must be called before this method
         *
         * @param unsigned long - The identifier of the item to retrieve
         *
         * @return IConfigItem* - The database object for this item. This will be NULL if one
         *                            is not found.
         */
        virtual TA_Base_Core::IConfigItem* getItem( unsigned long identifier );


        /**
         *
         * @param progress
         */
        std::map<CString, unsigned long> getItemNames( CProgressCtrl& progress );


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
         * loadItems
         *
         * This will retrieve all the train radio groups location from the database and store them in m_trainRadioGroups.
         *
         * @exception DataException - This will be thrown if there is an error with the train radio groupp data and it
         *                             cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();


        /**
         * Creates a new ite
         *
         * @return the new item
         */
        virtual TA_Base_Core::IConfigItem* newItem();


    private:

        /**
         *
         * @param theTrainRadioGroupDatabaseAccessor
         */
        TrainRadioGroupDatabaseAccessor( const TrainRadioGroupDatabaseAccessor& theTrainRadioGroupDatabaseAccessor );
        TrainRadioGroupDatabaseAccessor& operator=( const TrainRadioGroupDatabaseAccessor& pm );


        /**
         * Map of identifier to radio groups
         */
        typedef std::map<unsigned long, TA_IRS_Core::ITrainRadioGroupPtr> LoadedTrainRadioGroups;


        /**
         * whether the data is loaded or not
         */
        bool m_isLoaded;


        /**
         * This is all the train radio groups loaded from the database
         */
        LoadedTrainRadioGroups m_trainRadioGroups;

    };

}

#endif // !defined(TrainRadioGroupDatabaseAccessor_H)
