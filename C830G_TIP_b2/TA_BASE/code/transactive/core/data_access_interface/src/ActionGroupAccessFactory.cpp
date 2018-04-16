/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/ActionGroupAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * ActionGroupAccessFactory is a singleton that is used to retrieve ActionGroup objects either from the
  * database or newly created. All ActionGroup objects returned will adhear to the IActionGroup interface.
  * Data is primarily retrieved from the SE_ACTION_GROUP table
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ConfigActionGroup.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    ActionGroupAccessFactory::ActionGroupAccessFactory(): m_initialized(false)
	{
        //initializeActionGroup();
	}

	ActionGroupAccessFactory* ActionGroupAccessFactory::m_instance = 0;	

    ActionGroupAccessFactory& ActionGroupAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ActionGroupAccessFactory();
        }
        return *m_instance;
    }


	void ActionGroupAccessFactory::initializeActionGroup()
	{
        FUNCTION_ENTRY("ActionGroupAccessFactory::initializeActionGroup()");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		
        std::ostringstream sql;
        sql << "select seagro_id, seacti_id from se_action_group_map";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("SEACTI_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

		//TD17830 marc_bugfix
		//Make sure map is clear before adding entries
		m_actionGroupToActionMap.clear();

        do
        {
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long actionGroupId = data->getUnsignedLongData(i, columnNames[0]);
					unsigned long actionId = data->getUnsignedLongData(i, columnNames[1]);

					m_actionGroupToActionMap[actionGroupId].push_back(actionId);
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
	}

    void ActionGroupAccessFactory::getActionByActionGroup(unsigned long actionGroupId, std::vector<unsigned long>& actions)
    {
		FUNCTION_ENTRY("ActionGroupAccessFactory::getActionByActionGroup");

		ActionGroupToActionMap::iterator it = m_actionGroupToActionMap.find( actionGroupId );
		if ( it != m_actionGroupToActionMap.end() )
		{
            actions = it->second;
		}
		FUNCTION_EXIT;
    }
	
	void ActionGroupAccessFactory::getActionGroupBySql(const std::string& sql, std::vector<IActionGroup*>& actionGroups, const bool readWrite)
	{
		FUNCTION_ENTRY("getActionGroupBySql");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("IS_CONTROLTYPE");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

		//TD17830 marc_bugfix
		//Initialize Action Group to action map first
        if(!m_initialized)
        {
            initializeActionGroup();
            m_initialized = true;
        }
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( sql, columnNames );

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long actionGroupId = data->getUnsignedLongData(i, columnNames[0]);					
					std::vector<unsigned long> actions;
					getActionByActionGroup(actionGroupId, actions);

					if (readWrite)
					{
						actionGroups.push_back( new ConfigActionGroup(i, *data, actions) );
					}
					else
					{
						actionGroups.push_back( new ActionGroup(i, *data, actions) );
					}
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
            }

			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
	}

    IActionGroup* ActionGroupAccessFactory::getActionGroup( const unsigned long actionGroupKey, const bool readWrite )
    {
        FUNCTION_ENTRY("getActionGroup");

        std::ostringstream sql;
        sql << " select SEAGRO_ID, NAME, IS_CONTROLTYPE,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_ACTION_GROUP WHERE SEAGRO_ID = " << actionGroupKey;

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        std::vector< IActionGroup* > actionGroups;
        getActionGroupBySql(sql.str(), actionGroups, readWrite);

        if ( 0 == actionGroups.size())
        {
            std::ostringstream message;
            message << "No data found for actionGroup with key " << actionGroupKey;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == actionGroups.size(), "ActionGroup key unique constraint violated");

        FUNCTION_EXIT;
        return actionGroups[0];
    }


    std::vector<IActionGroup*> ActionGroupAccessFactory::getAllActionGroups(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllActionGroups");

        // create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select SEAGRO_ID, NAME, IS_CONTROLTYPE,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_ACTION_GROUP";

        std::vector< IActionGroup* > actionGroups;
        getActionGroupBySql(sql.str(), actionGroups, readWrite);

        FUNCTION_EXIT;
		return actionGroups;
    }


    IConfigActionGroup* ActionGroupAccessFactory::createActionGroup()
    {
        return new ConfigActionGroup();
    }


    IConfigActionGroup* ActionGroupAccessFactory::copyActionGroup(const IConfigActionGroup* actionGroupToCopy)
    {
        FUNCTION_ENTRY("copyActionGroup");

        TA_ASSERT(actionGroupToCopy !=  NULL, "The ActionGroup to copy was NULL");
        
        const ConfigActionGroup* cast = dynamic_cast<const ConfigActionGroup*>(actionGroupToCopy);

        TA_ASSERT(cast != NULL, "ActionGroup passed could not be converted into a ConfigActionGroup");

        FUNCTION_EXIT;
        return new ConfigActionGroup(*cast);
    }
    
    void ActionGroupAccessFactory::getAllActionGroupKeys(std::vector<unsigned long>& groupKeys)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::ostringstream sql;
        sql << "select seagro_id from se_action_group";
        IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
        
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    unsigned long actionGroupId = data->getUnsignedLongData(i, columnNames[0]);					
                    groupKeys.push_back(actionGroupId);
                    
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
            
            delete data;
            data = NULL;
        }
        while ( databaseConnection->moreData(data) );
        
        FUNCTION_EXIT;
    }
    
    std::string ActionGroupAccessFactory::getActionGroupName(const unsigned long key)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("NAME");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::ostringstream sql;
        sql << "select name from se_action_group where seagro_id = " << key;
        IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
        std::vector<unsigned long> rt;
        
        std::string groupName;
        TA_ASSERT(1==data->getNumRows(), "More than one name for one action group founded.");
        try
        {
            groupName = data->getStringData(0, columnNames[0]);					
            
        }
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }
        
        delete data;
        data = NULL;
        
        FUNCTION_EXIT;
        return groupName;
    }
    
    void ActionGroupAccessFactory::getAllActionGroupNames(std::map<unsigned long, std::string>& groupNames)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("NAME");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::ostringstream sql;
        sql << "select seagro_id, name from se_action_group";
        IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
        
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    unsigned long actionGroupId = data->getUnsignedLongData(i, columnNames[0]);	
                    std::string actionGroupName = data->getStringData(i, columnNames[1]);
                    groupNames.insert(std::make_pair(actionGroupId, actionGroupName));
                    
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
            
            delete data;
            data = NULL;
        }
        while ( databaseConnection->moreData(data) );
        
        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

