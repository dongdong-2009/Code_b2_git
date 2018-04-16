/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/sequence_entity/src/EntityDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve entity information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/sequence_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/sequence_entity/src/EntityDatabaseAccessor.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/IRegion.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    EntityDatabaseAccessor::EntityDatabaseAccessor(const std::vector<std::string>& entityTypes)
        : m_entityTypes(entityTypes),
          m_isLoaded(false)
    {
    }


    EntityDatabaseAccessor::~EntityDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedEntities::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void EntityDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");
		//int cntEntityType = 0;
		//int cntEntities = 0;
        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the entities and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedEntities::iterator entityToDelete = m_entities.begin(); entityToDelete != m_entities.end(); ++entityToDelete)
        {
            if( (entityToDelete->second) != NULL )
            {
                delete entityToDelete->second;
                entityToDelete->second = NULL;
            }
        }
        m_entities.clear();
		m_subStringItemNamesNumber.clear();

        // Now reload all the entities
        for ( std::vector<std::string>::iterator type = m_entityTypes.begin(); type != m_entityTypes.end(); ++type)
        {
            std::vector<TA_Base_Core::IEntityData*> entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType( (*type), true);
            // And insert the entities into our map. At the same time cast them to IConfigEntity*
            // instead of IEntityData*.
            for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entities.begin();
                 iter != entities.end();
                 ++iter
                 )
            {
					 
                 TA_Base_Core::IConfigEntity* castEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(*iter);
                 if( castEntity != NULL )
                 {
                    m_entities.insert( LoadedEntities::value_type(castEntity->getUniqueIdentifier(), castEntity) );
                 }
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> EntityDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");
		CString subStringItemNames;

        std::map<CString,unsigned long> names; 

        // The maximum range is the number of entities times by 2. This is because we want to step for each
        // entity while it is loaded and then again while each entity is added to the list.
        int currentPos = progress.GetPos();
        progress.SetRange( 0, (m_entities.size() * 2) + currentPos );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedEntities::iterator iter = m_entities.begin();
             iter != m_entities.end();
             ++iter)
        {
            if (iter->second != NULL)
            {	
				subStringItemNames = iter->second->getName().c_str();
				if(!subStringItemNames.IsEmpty())
					subStringItemNames = subStringItemNames.GetBufferSetLength(3);
				
				if(m_subStringItemNamesNumber.count(subStringItemNames)==0)
				{
					m_subStringItemNamesNumber.insert(std::map<CString,unsigned long>::value_type(subStringItemNames,1));
				}
				else
				{
					m_subStringItemNamesNumber[subStringItemNames] = (m_subStringItemNamesNumber.find(subStringItemNames))->second+1;
				}
                names.insert( std::map<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }
        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* EntityDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the entity matching the key passed in.
        LoadedEntities::iterator matching = m_entities.find(key);
        if (matching == m_entities.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the entity with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void EntityDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the entity matching the key passed in.
        LoadedEntities::iterator matching = m_entities.find(key);
        if (matching == m_entities.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the entity with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored entity was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisEntity();
        }
		CString tempName = (matching->second->getName()).c_str();
		tempName = tempName.GetBufferSetLength(3);
		m_subStringItemNamesNumber[tempName] = (m_subStringItemNamesNumber.find(tempName))->second-1;
        delete matching->second;
        matching->second = NULL;
        m_entities.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* EntityDatabaseAccessor::newItem()
    {
        TA_ASSERT(false,"This method should not be called. newItem(type) should be called instead");
        return NULL;
    }

        
    TA_Base_Core::IConfigItem* EntityDatabaseAccessor::newItem(const std::string& type)
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigItem* newEntity = TA_Base_Core::EntityAccessFactory::getInstance().createEntity(type);

        m_entities.insert( LoadedEntities::value_type( newEntity->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IConfigEntity*>(newEntity) ) );
		
		//CString tempName = m_entities.find(newEntity->getUniqueIdentifier())->second->getName().c_str();
		//tempName = tempName.GetBufferSetLength(3);
		//m_subStringItemNamesNumber[tempName] = (m_subStringItemNamesNumber.find(tempName))->second+1;
        return newEntity;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* EntityDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigEntity* newEntity = NULL;

        // Attempt to find the entity matching the key passed in.
        LoadedEntities::iterator matching = m_entities.find(idOfItemToCopy);

		CString tempName = matching->second->getName().c_str();
		tempName = tempName.GetBufferSetLength(3);
		unsigned long locCount = (m_subStringItemNamesNumber.find(tempName))->second;
		if(locCount<SEQ_COUNT)
		{
		m_subStringItemNamesNumber[tempName] = (m_subStringItemNamesNumber.find(tempName))->second+1;
        TA_ASSERT(matching != m_entities.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigEntity stored in the map");

        newEntity = TA_Base_Core::EntityAccessFactory::getInstance().copyEntity(matching->second);

        if (newEntity != NULL)
        {
            m_entities.insert( LoadedEntities::value_type( newEntity->getUniqueIdentifier(), newEntity ) );
        }
		}
        else
		{
			// TD14164 ++
			/*MessageBox(NULL,"Cannot Copy.....\n Sequence Entry Not More then 64","Error in Copy",MB_OK);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_010030);
			// ++ TD14164 
		}
        return newEntity;

        FUNCTION_EXIT;
    }


    std::map<unsigned long, std::string> EntityDatabaseAccessor::getAllLocations()
    {
        FUNCTION_ENTRY("getAllLocations");

        using TA_Base_Core::ILocation;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the locations from the database
        std::vector<ILocation*> locations;
	
        try
        {
            locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
		
        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ILocation object when we're done
		//m_subStringItemNamesNumber.clear();
        for (std::vector<ILocation*>::iterator iter = locations.begin(); iter != locations.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
					if((m_subStringItemNamesNumber.find((*iter)->getName().c_str()))->second<SEQ_COUNT)
					{
					namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
					}
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This location will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This location will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


	std::map<unsigned long, std::string> EntityDatabaseAccessor::getAllRegions()
    {
        FUNCTION_ENTRY("getAllRegions");

        using TA_Base_Core::IRegion;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the locations from the database
        std::vector<IRegion*> regions;
        try
        {
            regions = TA_Base_Core::RegionAccessFactory::getInstance().getAllRegions();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No regions will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No regions will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IRegion object when we're done
        for (std::vector<IRegion*>::iterator iter = regions.begin(); iter != regions.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This region will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This region will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> EntityDatabaseAccessor::getAllSubsystems()
    {
        FUNCTION_ENTRY("getAllSubsystems");
        std::map<unsigned long, std::string> namesAndKeys;

        using TA_Base_Core::ISubsystem;

        // First retrieve all the subsystems from the database
        std::vector<ISubsystem*> subsystems;
        try
        {
            subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllPhysicalSubsystems();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No subsystems will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No subsystems will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ISubsystem object when we're done
        for (std::vector<ISubsystem*>::iterator iter = subsystems.begin(); iter != subsystems.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This subsystem will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This subsystem will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    bool EntityDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& entitiesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(entitiesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }
        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedEntities::iterator iter = m_entities.begin(); iter != m_entities.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    entitiesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this entity that has not had it's changes applied yet");
                    entitiesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this entity that has not had it's changes applied yet");
                    entitiesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !entitiesNotApplied.empty();
    }
	void EntityDatabaseAccessor::increaseLocationCount(const CString& locName,const CString& prevText)
	{
		if((locName.CompareNoCase(prevText)!=0)&&prevText.Compare("")==0)
			m_subStringItemNamesNumber[locName] = (m_subStringItemNamesNumber.find(locName))->second+1;
		 if ((locName.CompareNoCase(prevText)!=0)&&(prevText.Compare("")!=0))
		 {
			m_subStringItemNamesNumber[prevText] = (m_subStringItemNamesNumber.find(prevText))->second-1;
			m_subStringItemNamesNumber[locName] = (m_subStringItemNamesNumber.find(locName))->second+1;
		 }

	}
}
