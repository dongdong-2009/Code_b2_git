/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/TrainStateDai/src/ITrain.h"

#include "core/data_access_interface/TrainStateDai/src/TrainAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Core;
using namespace TA_Base_Bus;


namespace
{
	const char TALKGROUP = 'B';
}


namespace TA_IRS_App
{


    TrTrainDatabaseAccessor::TrTrainDatabaseAccessor()
        : 
        m_isLoaded(false)
    {
        m_configItems.clear();
    }


    TrTrainDatabaseAccessor::~TrTrainDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configItems.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }		

        FUNCTION_EXIT;
    }


    void TrTrainDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Tr_train and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (ConfigItemsIt assocToDelete = m_configItems.begin(); assocToDelete != m_configItems.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configItems.clear();

		std::vector<ITrain*> items;
        try
		{
			// Now reload all the train
			 items =  TrainAccessFactory::getInstance().getAllTrains(true);  
		}
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No Locations will be added to the map. An empty map will be returned");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No Locations will be added to the map. An empty map will be returned");
        }

		// And insert the train into our map. At the same time cast them to IConfigTrain*
        // instead of ITrain*.
        for (std::vector<ITrain*>::iterator it = items.begin(); it != items.end(); ++it)
        {
           
            if( (*it) != NULL )
            {
                unsigned long key = (*it)->getUniqueIdentifier();
                m_configItems[key] = *it;
            }
        }

        refreshCachedData();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> TrTrainDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord> TrTrainDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long, CfgTrTrainRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_configItems.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                CfgTrTrainRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, CfgTrTrainRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    TrTrainDatabaseAccessor::CfgTrTrainRecord TrTrainDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the train with identifier %lu",identifier);
            FUNCTION_EXIT;
            CfgTrTrainRecord record;
            return record;
        }

        CfgTrTrainRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    TrTrainDatabaseAccessor::CfgTrTrainRecord TrTrainDatabaseAccessor::getRecordFrom(TA_IRS_Core::ITrain& item)
    {
        CfgTrTrainRecord record;

//         if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getKey()))
//         {
//             record.key = "";
//         }
//         else
//         {
//             record.key.Format("%ld", item.getKey()); 
//         }
		
		if(TA_Base_Core::DatabaseKey::isInvalidKey(  item.getTrainId()  ) )
		{
			record.trainId = "";
		}
		else
		{
			record.trainId.Format("%ld", item.getTrainId());
		}
				
		record.primaryTsi = ( item.getPrimaryTsi().c_str() );

		record.secondaryTsi = ( item.getSecondaryTsi().c_str() );
		
	    return record;
    }

    
    TA_Base_Core::IConfigItem* TrTrainDatabaseAccessor::getItem(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the train with key %lu",identifier);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void TrTrainDatabaseAccessor::deleteItem(unsigned long identifier)
    {
		
		FUNCTION_ENTRY("deleteItem");
		
        // Attempt to find the Ats Fixed Block Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the train talk group with key %lu",identifier);
            FUNCTION_EXIT;
        }
		
        TA_ASSERT(matching->second != NULL, "Stored train was equal to NULL");
		
        if (!matching->second->isNew())
        {
            // TODO - check asserts
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_configItems.erase(matching);
		
        FUNCTION_EXIT;
		
    }


    TA_Base_Core::IConfigItem* TrTrainDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::ITrain* newTrain = TA_IRS_Core::TrainAccessFactory::getInstance().createTrain();

        m_configItems.insert( ConfigItems::value_type( newTrain->getUniqueIdentifier(), newTrain ) );
		 //m_configItems.insert( ConfigItems::value_type( newTrain->getTrainId(), newTrain ) );

        return newTrain;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* TrTrainDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::ITrain* newAssoc = NULL;

        // Attempt to find the train talk group matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigTrTalkGroup stored in the map");

        newAssoc = TA_IRS_Core::TrainAccessFactory::getInstance().copyTrain(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
			//m_configItems.insert( ConfigItems::value_type( newAssoc->getTrainId(), newAssoc ) );
        }

        FUNCTION_EXIT;

		return newAssoc;
    }



    bool TrTrainDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end();  ++it)
        {
            if( (it->second != NULL) && (it->second->hasChanged() ) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for PA Zones
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void TrTrainDatabaseAccessor::refreshCachedData()
    {
        FUNCTION_ENTRY("refreshCachedData");


        FUNCTION_EXIT;
    }

    const TA_Base_App::MultipleChangeDialog::ItemsForSelection TrTrainDatabaseAccessor::getDataForSelection() 
    {
		TA_Base_App::MultipleChangeDialog::ItemsForSelection result;
		
		return result;
    }

    std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties>
    TrTrainDatabaseAccessor::getDataSetHeaderProperties()
    {
        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> props;

		return props;
    }
}

    
