/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/ActionGroupHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * ActionGroupHelper is an object that is held by all type-specific ActionGroup objects, as well as by
  * the ConfigActionGroup. It contains all data common across ActionGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for each ActionGroup object.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#pragma warning(disable:4284)
#endif

#include <ctime>
#include <vector>

#include "core/data_access_interface/src/ActionGroupHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    ActionGroupHelper::ActionGroupHelper( )
        : m_name(""),
          m_isControlType(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    ActionGroupHelper::ActionGroupHelper( const unsigned long key, const std::string& name )
        : m_key(key),
          m_name(name),
          m_isControlType(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(false),
          m_isValidData(false)
    {
    }


    ActionGroupHelper::ActionGroupHelper(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions)
    :   m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("ActionGroupHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data, actions);
        FUNCTION_EXIT;
    }


    ActionGroupHelper::ActionGroupHelper( const ActionGroupHelper& theActionGroup)
        : m_key(theActionGroup.m_key),
          m_name(theActionGroup.m_name),
          m_isControlType(theActionGroup.m_isControlType),
          m_dateCreated(0),
          m_dateModified(0),
          m_actions( theActionGroup.m_actions),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    ActionGroupHelper::~ActionGroupHelper()
    {
    }


    unsigned long ActionGroupHelper::getKey()
    {
        TA_ASSERT(!m_isNew, "Cannot retrieve the key on a ActionGroup before it has been applied to the database");
        return m_key;
    }

    
    std::string ActionGroupHelper::getName()
    {
        // We don't want to call reload() if this is a new ActionGroup and it has not been written to the database
        if (!m_isValidData && !m_isNew && m_name.empty())  
        {            
            // This is an existing ActionGroup and the data needs to be loaded
            reload();
        }
        return m_name;
    }

    
    bool ActionGroupHelper::isControlType()
    {
        // We don't want to call reload() if this is a new ActionGroup and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing ActionGroup and the data needs to be loaded
            reload();
        }
        return m_isControlType;
    }
    
    
    bool ActionGroupHelper::isActionIncluded(unsigned long actionKey)
    {
        // If we search and something is returned other than m_actions.end() then the action is already included
        return (m_actions.find(actionKey) != m_actions.end());
    }


    time_t ActionGroupHelper::getDateCreated()
    {
        // We don't want to call reload() if this is a new ActionGroup and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing ActionGroup and the data needs to be loaded
            reload();
        }
        return m_dateCreated;
    }


    time_t ActionGroupHelper::getDateModified()
    {
        // We don't want to call reload() if this is a new ActionGroup and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing ActionGroup and the data needs to be loaded
            reload();
        }
        return m_dateModified;
    }

 
    void ActionGroupHelper::setName(const std::string& name)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
    }


    void ActionGroupHelper::setIsControlType(bool isControlType)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isControlType = isControlType;
    }

        
    void ActionGroupHelper::addAction(unsigned long actionKey)
    {
        FUNCTION_ENTRY("addAction");

        if ( m_actions.find(actionKey) == m_actions.end() )
        {
            // This means the key isn't currently in the list so we must add it
            m_actions.insert( actionKey );
        }

        FUNCTION_EXIT;
    }


    void ActionGroupHelper::removeAction(unsigned long actionKey)
    {
        FUNCTION_ENTRY("removeAction");

        m_actions.erase( actionKey );

        FUNCTION_EXIT;
    }

        
    void ActionGroupHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;
        m_name = "";
        m_actions.clear();

        FUNCTION_EXIT;
    }


    void ActionGroupHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("ActionGroup data not fully specified. ActionGroup cannot be written to database" ,fieldNames) );
        }

        // Write the ActionGroup to the database

        // Need to check if this is a new ActionGroup or an existing ActionGroup - 
        // as a new ActionGroup will be inserted, while an existing ActionGroup will be updated
        if (m_isNew)
        {
             // This is a new ActionGroup
            addNew();
        }
        else 
        {
            // This is an existing ActionGroup
            modifyExisting();
        }

        updateActions();

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void ActionGroupHelper::deleteThisActionGroup()
    {
        FUNCTION_ENTRY("deleteThisActionGroup");

        TA_ASSERT(!m_isNew, "This ActionGroup does not yet exist in the database, and therefore cannot be deleted");
        
        checkForLinkedProfiles(); // This will throw an exception if we can't delete

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // We must now attempt to delete the ActionGroup

        std::ostringstream sql;
        sql << "delete SE_ACTION_GROUP_MAP where SEAGRO_ID = " << m_key;

        databaseConnection->executeModification(sql.str());

        sql.str("");
        sql << "delete SE_ACTION_GROUP where SEAGRO_ID = " << m_key;

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }

	void ActionGroupHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("IS_CONTROLTYPE");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}
    
    void ActionGroupHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        std::ostringstream sql;
        sql << " select SEAGRO_ID, NAME, IS_CONTROLTYPE,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_ACTION_GROUP WHERE SEAGRO_ID = " << m_key;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( sql.str(), columnNames ));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No ActionGroup found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of ActionGroup records.");

        sql.str("");
        sql << "select seacti_id from se_action_group_map where seagro_id = " << m_key;

        // Set up the columnNames vector to be passed to executeQuery()
        columnNames.clear();
        columnNames.push_back("SEACTI_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data2 = databaseConnection->executeQuery(sql.str(), columnNames);
		std::vector< unsigned long > actions;

        do
        {
			for (unsigned long i = 0; i < data2->getNumRows(); i++)
			{
				try
				{
					unsigned long actionId = data2->getUnsignedLongData(i, columnNames[0]);
					actions.push_back(actionId);
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data2;
					data2 = NULL;
					throw;
				}
			}
			delete data2;
			data2 = NULL;
        }
        while ( databaseConnection->moreData(data2) );

        reloadUsing(0, *data, actions);
        FUNCTION_EXIT;
    }

    void ActionGroupHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions)
    {
        FUNCTION_ENTRY("reloadUsing");

		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key	= data.getUnsignedLongData( row, columnNames[0] );
		m_name	= data.getStringData( row, columnNames[1] );
		m_isControlType= data.getBooleanData( row, columnNames[2] );
		m_dateModified = data.getDateData(row, columnNames[3], 0);
		m_dateCreated  = data.getDateData( row, columnNames[4]);

        std::vector<unsigned long>::iterator iter;
		for (iter = actions.begin(); iter != actions.end(); iter++)
		{
			m_actions.insert(*iter);
		}

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void ActionGroupHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "ActionGroup %s already exists. It's details will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        std::string formatSQL = "select SEAGRO_ID from SE_ACTION_GROUP where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("KEY");

        // execute the query
        IData* data = databaseConnection->executeQuery(formatSQL, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0, columnNames[0]))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An ActionGroup already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
        }

        delete data;
        data =  NULL;

        std::ostringstream sql;
        sql << "update SE_ACTION_GROUP set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', IS_CONTROLTYPE = " << m_isControlType;
        sql << " where SEAGRO_ID = " << m_key;

        databaseConnection->executeModification(sql.str());


        //now need to update the date fields
        sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from SE_ACTION_GROUP where SEAGRO_ID = " << m_key;

        // Can use the same columnNames vector from above.
        columnNames.clear();
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        try
        {
            m_dateCreated  = data->getDateData(0, columnNames[0]);
            m_dateModified = data->getDateData(0, columnNames[1], 0);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;
        FUNCTION_EXIT;
    }

        
    void ActionGroupHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "ActionGroup %s is new. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // First check there is not alreay an entity with this name (it is a uniqueness
        // constraint).
        std::ostringstream formatSQL;
        formatSQL << "select SEAGRO_ID from SE_ACTION_GROUP where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";


        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(formatSQL.str(),columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A ActionGroup already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        formatSQL.str("");
        formatSQL << "insert into SE_ACTION_GROUP (SEAGRO_ID,NAME,IS_CONTROLTYPE) ";
        formatSQL << "values (SECURITY_SEQ.NEXTVAL,'";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "',";
		formatSQL << m_isControlType << ")";
 
        databaseConnection->executeModification(formatSQL.str());

        //now need to get the PKEY back out for this location and update the date fields
        std::ostringstream sql;
		sql << "select SEAGRO_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SE_ACTION_GROUP where NAME = '";
        sql << databaseConnection->escapeQueryString(m_name).c_str() << "'";

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void ActionGroupHelper::updateActions()
    {
        FUNCTION_ENTRY("updateActions");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        std::ostringstream sql;
        sql << "select SEACTI_ID from SE_ACTION_GROUP_MAP where SEAGRO_ID = " << m_key;

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string actionKeyColumn = "KEY";
        columnNames.push_back(actionKeyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        std::set<unsigned long> localActionsToAdd = m_actions;
        std::vector<unsigned long> databaseActionsToRemove;

        // Now we step through the keys retrieved from the database.
        // If we find the key in both lists then the database is up-to-date and nothing needs to be done to the
        // lists (ie we dont' add it to the databaseActionToRemove list). At the end the keys left in the localActionsToAdd
        // list need to be added to the database and the actions left in databaseActionsToRemove must be removed from the
        // database.
        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                unsigned long currentKey = data->getUnsignedLongData(i, actionKeyColumn);

                if (localActionsToAdd.find( currentKey ) != localActionsToAdd.end())
                {
                    // This key has been found so it has not changed
                    // We remove it from the localActionsToRemove list as we do not need to remove it from the database
                    localActionsToAdd.erase( currentKey );
                }
                else
                {
                    // This key has obviously been removed from our local list so it will need to be removed from the database
                    databaseActionsToRemove.push_back( currentKey );
                }
            }

            delete data;
            data = NULL;
        }
		while (databaseConnection->moreData( data ) );

        // Now we add and delete from/to the database as required
       
        // First lets delete old values from the database
        if (!databaseActionsToRemove.empty())
        {
            std::ostringstream sql;
            sql << "delete SE_ACTION_GROUP_MAP where SEAGRO_ID = " << m_key << " and SEACTI_ID in (";
            for (std::vector<unsigned long>::iterator iterToRemove = databaseActionsToRemove.begin();
                 iterToRemove != databaseActionsToRemove.end(); ++iterToRemove)
            {
                if ( iterToRemove != databaseActionsToRemove.begin())
                {
                    // If this is not the first item then we must insert a comma
                    sql << ",";
                }
                sql << *iterToRemove;
            }    
            sql << ")";

            databaseConnection->executeModification(sql.str().c_str());
        }

        // Now lets add new values to the database
        for (std::set<unsigned long>::iterator iterToAdd = localActionsToAdd.begin();
             iterToAdd != localActionsToAdd.end(); ++iterToAdd)
        {
             std::ostringstream sql;
             sql << "insert into SE_ACTION_GROUP_MAP(SEAGRO_ID, SEACTI_ID) ";
             sql << "values(" << m_key << "," << *iterToAdd << ")";

            databaseConnection->executeModification(sql.str().c_str());
        }

        FUNCTION_EXIT;
    }


    void ActionGroupHelper::checkForLinkedProfiles()
    {
        FUNCTION_ENTRY("checkForLinkedProfiles");

		const unsigned long CHILD_ENTITIES=10;
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // If there are any entities with this entity as the parent key or agent then we cannot delete
        // until they have been moved.

        std::ostringstream sql;
        sql << "select unique(p.name) from SE_PROFILE p, SE_PROFILE_ACCESS a where a.SEAGRO_ID = " << m_key;
        sql << " and a.SEPROF_ID = p.SEPROF_ID";

        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames,CHILD_ENTITIES+1);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        if (data->getNumRows() > 0)
        {
            std::string linkedProfiles("");
            // Only display the first ten items that we receive. If there are more then show
            // this using "...<more>".
            for( unsigned int i = 0; (i < data->getNumRows() && i < CHILD_ENTITIES); ++i)
            {
                linkedProfiles += "Profile ";
                linkedProfiles += data->getStringData(i,nameColumn);
                linkedProfiles += "\n";
            }

            if (data->getNumRows() > CHILD_ENTITIES)
            {
                linkedProfiles += "...<more>\n";
            }

            delete data;

            TA_THROW( DataException("This ActionGroup cannot be deleted as it has profiles using it.",
                                    DataException::CANNOT_DELETE,
                                    linkedProfiles ) );
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core


