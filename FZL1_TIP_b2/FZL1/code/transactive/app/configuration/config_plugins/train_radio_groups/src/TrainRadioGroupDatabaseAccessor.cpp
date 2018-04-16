/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/trainradiogroupdatabaseaccessor.cpp $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This class uses the Data Access Interface to retrieve system controller information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */

#include "app/configuration/config_plugins/train_radio_groups/src/stdafx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDatabaseAccessor.h"


#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroupAccessFactory.h"
#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroup.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;


    TrainRadioGroupDatabaseAccessor::TrainRadioGroupDatabaseAccessor()
        : m_isLoaded( false )
    {
        FUNCTION_ENTRY( "TrainRadioGroupDatabaseAccessor" );
        FUNCTION_EXIT;
    }


    TrainRadioGroupDatabaseAccessor::~TrainRadioGroupDatabaseAccessor()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupDatabaseAccessor" );

        // clear the map, it releases the shared pointers
        m_trainRadioGroups.clear();

        FUNCTION_EXIT;
    }


    bool TrainRadioGroupDatabaseAccessor::areCurrentChangesPending( std::vector<std::string>& itemsNotApplied )
    {
        FUNCTION_ENTRY( "areCurrentChangesPending" );

        TA_ASSERT( itemsNotApplied.empty(), "the vector of string is not empty" );

        // If the data has not been loaded yet then there can't be any changes pending
        if ( false == m_isLoaded )
        {
            FUNCTION_EXIT;
            return false;
        }

        for ( LoadedTrainRadioGroups::iterator iter = m_trainRadioGroups.begin(); iter != m_trainRadioGroups.end(); ++iter )
        {
            TA_ASSERT( iter->second, "This map cannot have an empty pointer in it" );

            if ( true == iter->second->hasChanged() )
            {
                try
                {
                    itemsNotApplied.push_back( iter->second->getName() );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this radio group that has not had it's changes applied yet" );
                    itemsNotApplied.push_back( "Unknown" );
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this radio group that has not had it's changes applied yet" );
                    itemsNotApplied.push_back( "Unknown" );
                }
            }
        }

        FUNCTION_EXIT;
        return ( false == itemsNotApplied.empty() );
    }


    TA_Base_Core::IConfigItem* TrainRadioGroupDatabaseAccessor::copyItem( const unsigned long idOfItemToCopy )
    {
        FUNCTION_ENTRY( "copyItem" );

        TA_IRS_Core::ITrainRadioGroupPtr newTrainRadioGroup;

        // Attempt to find the ITSI matching the key passed in.
        LoadedTrainRadioGroups::iterator matching = m_trainRadioGroups.find( idOfItemToCopy );

        TA_ASSERT( matching != m_trainRadioGroups.end(), "The key passed could not be found in the map" );
        TA_ASSERT( matching->second, "There was a NULL ConfigTrainRadioGroup stored in the map" );

        try
        {
            newTrainRadioGroup = TA_IRS_Core::TrainRadioGroupAccessFactory::copyTrainRadioGroup( matching->second );

            m_trainRadioGroups.insert( LoadedTrainRadioGroups::value_type( newTrainRadioGroup->getUniqueIdentifier(), newTrainRadioGroup ) );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not create newTrainRadiopGroup" );
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not create new TrainRadioGroup" );
        }


        FUNCTION_EXIT;
        return newTrainRadioGroup.get();
    }


    void TrainRadioGroupDatabaseAccessor::deleteItem( unsigned long id )
    {
        FUNCTION_ENTRY( "deleteItem" );

        LoadedTrainRadioGroups::iterator matching  = m_trainRadioGroups.find( id );

        if ( matching == m_trainRadioGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the TrainRadio group with key %lu", id );
            FUNCTION_EXIT;
            return;
        }

        TA_ASSERT( matching->second, "Stored TrainRadio group was equal to NULL" );

        // delete it from the database
        matching->second->deleteThisItem();

        // erase it from the list
        m_trainRadioGroups.erase( matching );

        FUNCTION_EXIT;
    }


    std::map<unsigned long, std::string> TrainRadioGroupDatabaseAccessor::getAllLocations()
    {
        FUNCTION_ENTRY( "getAllLocations" );

        // note this loads every time so in case location changes were made in the location plugin
        // they will be picked up here without restarting the config editor

        std::map<unsigned long, std::string> allLocations;

        std::vector<TA_Base_Core::ILocation*> locationList;

        try
        {
            locationList = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No location will be added to the map. An empty map will be returned" );
            FUNCTION_EXIT;
            return allLocations;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No location will be added to the map. An empty map will be returned" );
            FUNCTION_EXIT;
            return allLocations;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the IEntityData object when we're done
        for ( std::vector<TA_Base_Core::ILocation*>::iterator iter = locationList.begin();
              iter != locationList.end(); ++iter )
        {
            try
            {
                // filter out anything that isnt a station or depot location (ie OCC/TDS)
                TA_Base_Core::ILocation::ELocationType locationType = ( *iter )->getLocationType();

                if ( ( locationType == TA_Base_Core::ILocation::STATION ) ||
                     ( locationType == TA_Base_Core::ILocation::DPT ) )
                {
                    allLocations.insert( std::map<unsigned long, std::string>::value_type( ( *iter )->getKey(), ( *iter )->getName() ) );
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This server will not be added to the map" );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This server will not be added to the map" );
            }

            delete *iter;
            *iter = NULL;
        }

        FUNCTION_EXIT;
        return allLocations;
    }


    TA_Base_Core::IConfigItem* TrainRadioGroupDatabaseAccessor::getItem( unsigned long identifier )
    {
        FUNCTION_ENTRY( "getItem" );

        LoadedTrainRadioGroups::iterator iter = m_trainRadioGroups.find( identifier );

        if ( iter == m_trainRadioGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ITSI group with key %lu", identifier );
            FUNCTION_EXIT;
            return NULL;
        }

        TA_IRS_Core::ITrainRadioGroup* groupPointer = iter->second.get();

        // upcasts dont require a cast
        TA_Base_Core::IConfigItem* configItem = groupPointer;
        
        FUNCTION_EXIT;
        return configItem;
    }


    std::multimap<CString, unsigned long> TrainRadioGroupDatabaseAccessor::getItemNames( CProgressCtrl& progress )
    {
        FUNCTION_ENTRY( "getItemNames" );

        std::multimap<CString, unsigned long> names;
        progress.SetRange( 0, m_trainRadioGroups.size() );
        progress.SetStep( 1 );

        for ( LoadedTrainRadioGroups::iterator iter = m_trainRadioGroups.begin();
              iter != m_trainRadioGroups.end(); ++iter )
        {
            unsigned long unKey = iter->second->getUniqueIdentifier();
            std::string strName = iter->second->getName();
            names.insert( std::multimap<CString, unsigned long>::value_type( strName.c_str(), unKey ) );
            progress.StepIt();
        }

        FUNCTION_EXIT;
        return names;
    }


    void TrainRadioGroupDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY( "loadItems" );

        if ( m_isLoaded )
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "We have already loaded the TrainRadioGroup and this is not a refresh so we exit" );
            FUNCTION_EXIT;
            return;
        }

        if ( m_trainRadioGroups.size() > 0 )
        {
            m_trainRadioGroups.clear();
        }

        std::vector<TA_IRS_Core::ITrainRadioGroupPtr> ITrainRadioGroupVec = TA_IRS_Core::TrainRadioGroupAccessFactory::loadAllTrainRadioGroups();

        for ( std::vector<TA_IRS_Core::ITrainRadioGroupPtr>::iterator trainiter = ITrainRadioGroupVec.begin();
              trainiter != ITrainRadioGroupVec.end(); ++trainiter )
        {
            unsigned long unkey = ( *trainiter )->getUniqueIdentifier();
            m_trainRadioGroups.insert( LoadedTrainRadioGroups::value_type( unkey, *trainiter ) );
        }

        m_isLoaded = true;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* TrainRadioGroupDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY( "newItem" );

        TA_IRS_Core::ITrainRadioGroupPtr newTrainRadioGroup = TA_IRS_Core::TrainRadioGroupAccessFactory::createNewTrainRadioGroup();

        TA_ASSERT( newTrainRadioGroup, "newTrainRadioGroup is not empty" );

        m_trainRadioGroups.insert( LoadedTrainRadioGroups::value_type( newTrainRadioGroup->getUniqueIdentifier(), newTrainRadioGroup ) );

        FUNCTION_EXIT;
        return newTrainRadioGroup.get();
    }

}