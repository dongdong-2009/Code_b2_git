/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This class uses the Data Access Interface to retrieve Video Station information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(TrTrainDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define TrTrainDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

#include "core/data_access_interface/TrainStateDai/src/TrainAccessFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ItemKeyToNameMap.h"
#include "core/data_access_interface/src/DatabaseKey.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_IRS_Core
{
    class ITrain;
// 	class IRadioSubscriber;
}

namespace TA_IRS_App
{
    class TrTrainDatabaseAccessor  :  public TA_Base_App::IDatabaseAccessor 
    {

    public:
		     
   
        struct CfgTrTrainRecord
        {
            CString status;             // This will be the current applied status of the item (ie new, modified etc)
                                        // It will be set elsewhere so we don't worry about it
            //CString key;    
            
            CString trainId;                            
            CString primaryTsi;
            CString secondaryTsi;
          
        };
        
        /**
         * Constructor
         */
        TrTrainDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~TrTrainDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the si_train records from the database and stores them locally.
         *
         * @exception DataException - This will be thrown if there is an error with the si_train
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
         * @return map<unsigned long, ConfigPaDvaMessage> - This is all the si_train keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long, CfgTrTrainRecord> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return ConfigPaDvaMessage - This is all the si_train details
         */
        CfgTrTrainRecord getItemDetails(unsigned long identifier);
 
        
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
         * getRecordFrom
         *
         * Converts data from IConfigSiTrain into ConfigPaDvaMessage structure
         */
        CfgTrTrainRecord getRecordFrom(TA_IRS_Core::ITrain& item);
        
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
        const TA_Base_App::MultipleChangeDialog::ItemsForSelection getDataForSelection();
    
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
        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> getDataSetHeaderProperties();


    protected:
        
        /**
         * refreshCachedData
         *
         * Refreshes local cache data (called once off)
         *
         */
        void refreshCachedData();
   
    private:

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        TrTrainDatabaseAccessor( const TrTrainDatabaseAccessor& theTrTrainDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        TrTrainDatabaseAccessor& operator=(const TrTrainDatabaseAccessor& pm);
 

        typedef std::map<unsigned long, TA_IRS_Core::ITrain*>			ConfigItems;
        typedef ConfigItems::iterator                                  ConfigItemsIt;

        ConfigItems   m_configItems;      // This is all the si_train loaded from the database
        
        bool m_isLoaded;                // This indicates if we have loaded the si_train before or not.
                                        // If we have never loaded the si_train then when we are told
                                        // to refresh we shouldn't load them.
// 		std::vector<TA_IRS_Core::IRadioSubscriber*> m_radioSubscribers;
    };
}

#endif // !defined(TrTrainDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
