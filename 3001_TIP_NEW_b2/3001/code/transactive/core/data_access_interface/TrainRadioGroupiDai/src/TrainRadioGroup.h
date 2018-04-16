/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $14:14 2007-11-5$
  * Last modified by:  $Lan Yuan$
  *
  */
#if !defined(TrainRadioGroup_H)
#define TrainRadioGroup_H


#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"
#include "core/data_access_interface/src/IData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>

namespace TA_IRS_Core
{
    /**
     * This is the concrete class that is used to edit/save the Train Radio Groups and
     * their coverage areas.
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:16 PM
     */
    class TrainRadioGroup : public ITrainRadioGroup
    {

    public:
        
        
        /**
         * Creates a new, empty group
         */
        TrainRadioGroup();


       /**
         * Creates a new, group as a copy of another group
         * 
         * @param rhs
         */
        TrainRadioGroup(const TrainRadioGroup& rhs);


        /**
         * Creates a group from existing data.
         * 
         * Each row of the IData will be processed, until a new group ID is found, in
         * which case this signals the start of a new group.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param data    This is the raw data, the query used to obtain this data must
         * order by the group tsi, so all consecutive records for a group are sequential
         * in this list.
         * @param row    This is the starting row number, when completed, the given
         * parameter will point to the next record.
         */
        TrainRadioGroup(TA_Base_Core::IData& data, unsigned long& row);


        /**
         * This will load all Train Radio Group configuration from the database
         * 
         * @return a list of all train radio groups
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        static std::vector< ITrainRadioGroupPtr > loadAllTrainRadioGroups();


        /**
         * This will load a single Train Radio Group configuration from the database
         * 
         * @return a train radio group if there is any matching (exception otherwise)
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param pkey    The group PKEY to load
         */
        static ITrainRadioGroupPtr loadTrainRadioGroup(unsigned long pkey);


        /**
         * This will load a single Train Radio Group configuration from the database
         * 
         * @return a train radio group if there is any matching (exception otherwise)
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param groupTsi    The group TSI to load
         */
        static ITrainRadioGroupPtr loadTrainRadioGroup(const std::string& groupTsi);


        /**
         * Returns the key for this item.
         * 
         * @return The key for this item as an unsigned long.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned long getKey();


        /**
         * Returns the name of this item. If this is the first time data for this item has
         * been requested, all the data is loaded from the database.
         * 
         * @return The name for this item as a std::string
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual std::string getName();


        /**
         * Gets the TSI this group represents
         * 
         * @return the TSI for this group
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual std::string getTsi();


        /**
         * Gets the Locations this group covers
         * 
         * @return the list of locations for this group
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual ITrainRadioGroup::LocationSet getLocationCoverage();


        /**
         * Sets the name of this item locally.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param name    The name to give this item.
         */
        virtual void setName(const std::string& name);


        /**
         * Sets the TSI this group represents
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param tsi    The new TSI
         */
        virtual void setTsi(const std::string& tsi);


        /**
         * Sets the Locations this group covers.
         * Overwrites any previous settings
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param locations    The new location list
         */
        virtual void setLocationCoverage(const ITrainRadioGroup::LocationSet& locations);


        /**
         * Adds a new location to the list of current covered locations.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param location    Adds a location to the current coverage
         */
        virtual void addLocation(unsigned long location);


        /**
         * Removes a location to the list of current covered locations.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param location    Adds a location to the current coverage
         */
        virtual void removeLocation(unsigned long location);


        /**
         * Make the data contained by this item as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


        /**
         * This retrieves the unqiue identifier for this item. We cannot use keys for
         * identification in the configuration editor because new items do not have keys.
         * Therefore we use this method for identification
         * 
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier();


        /**
         * This is called to retrieve all changes made to the item. If the map returned is
         * empty then no changes have been made. This must be called before the changes
         * are applied or it will be cleared.
         * 
         * @return ItemChanges - The map containing all item changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges();


        /**
         * This should be called to determine if any part of the item has been changed by
         * the user.
         * 
         * @return bool - This will return true if the item does not match the one in the
         * database. It will return false if the data has not changed from that in the
         * database.
         */
        virtual bool hasChanged();


        /**
         * This should be called to determine if this item is new. A new item is one that
         * has not yet been applied to the database and been given a pkey and create date.
         * 
         * 
         * @return bool - This will return true if the item has not been applied to the
         * database.
         */
        virtual bool isNew();


        /**
         * This will apply all changes made to the database.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual void applyChanges();


        /**
         * Will delete this group from the database
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual void deleteThisItem();

        
    private:
            

       /**
         * Creates and executes a query to load data by pkey
         * 
         * @return the IData containing the results, it is the responsibility of the
         * caller to delete
         * 
         * @exception DataException if there was a problem with the data, or there are no
         * results
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param pkey    The PKEY of the group to load
         */
        static TA_Base_Core::IData* loadDataByPkey(unsigned long pkey);


        /**
         * Creates and executes a query to load data by group TSI
         * 
         * @return the IData containing the results, it is the responsibility of the
         * caller to delete
         * 
         * @exception DataException if there was a problem with the data, or there are no
         * results
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param groupTsi    The TSI of the group to load
         */
        static TA_Base_Core::IData* loadDataByGroupTsi(const std::string& groupTsi);


        /**
         * This gets the column names to be used when decoding the data
         */
        static const std::vector<std::string>& getColumnNames();

        
        /**
         * This gets the column names to be used when decoding the data
         */
        static const std::vector<std::string>& getRadioSubscriberColumnNames();


        /**
         * Reloads the data from the database if necessary
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        void reload();


        /**
         * sets the members from existing data.
         * 
         * Each row of the IData will be processed, until a new group ID is found, in
         * which case this signals the start of a new group.
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param data    This is the raw data, the query used to obtain this data must
         * order by the group tsi, so all consecutive records for a group are sequential
         * in this list.
         * @param row    This is the starting row number, when completed, the given
         * parameter will point to the next record.
         */
        void loadData(TA_Base_Core::IData& data, unsigned long& row);


        /**
         * Will look up this group TSI in the RA subscribers table, and load the name if
         * it exists
         */
        void loadSubscriberName();

        
        /**
         * These are constants to define the database structure
         */
        static const std::string TR_RADIO_GROUP_TABLE;
        static const std::string TR_GROUP_COVERAGE_TABLE;
        static const std::string GROUP_PKEY_COLUMN;
        static const std::string GROUP_TSI_COLUMN;
        static const std::string LOCATION_COLUMN;
        static const std::string RA_SUBSCRIBERS_TABLE;
        static const std::string SUBSCRIBER_ID_COLUMN;
        static const std::string SUBSCRIBER_NAME_COLUMN;

        
        /**
         * The sequence used to generate the next pkey when saving a new group
         */
        static const std::string GROUP_PKEY_SEQUENCE;


        /**
         * lock for initialisation of static column names vector
         */
        static TA_Base_Core::NonReEntrantThreadLockable s_columnNamesLock;


        /**
         * The vector of column names used to load data from the database
         */
        static std::vector<std::string> s_columnNames;


        /**
         * The vector of column names used to load data from the database
         */
        static std::vector<std::string> s_radioSubscriberColumnNames;


        /**
         * used to initialise m_uniqueIdentifier
         */
        static unsigned long s_uniqueIdentifier;


        /**
         * Used by the config editor to identify things without unique PKEYs
         */
        unsigned long m_uniqueIdentifier;


        /**
         * Whether the item is new or not
         */
        bool m_isNew;


        /**
         * Whether the data is valid (true) or dirty and requires reload (false)
         */
        bool m_isValidData;


        /**
         * The list of item changes
         */
        TA_Base_Core::ItemChanges m_itemChanges;


        /**
         * The primary key of the group
         */
        unsigned long m_pkey;


        /**
         * This is the TSI of the group
         */
        std::string m_groupTsi;


        /**
         * This is the location coverage of the group
         */
        ITrainRadioGroup::LocationSet m_locationCoverage;


        /**
         * This is the name of the group - derived from NMS data if it exists
         */
        std::string m_name;
        
    };//end of class TrainRadioGroup
    
}// end of TA_IRS_Core
#endif // !defined(TrainRadioGroup_H)
