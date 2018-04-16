/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#pragma warning(disable:4786)

#include "ManagedProcessAccessFactory.h"
#include "IManagedProcessData.h"
#include "ManagedProcessData.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_Base_Core
{

    ManagedProcessAccessFactory* ManagedProcessAccessFactory::m_instance = 0;

    ManagedProcessAccessFactory& ManagedProcessAccessFactory::getInstance()
    {
        if (m_instance == NULL)
        {
            m_instance = new ManagedProcessAccessFactory();
        }
        return *m_instance;
    }

    std::vector<IManagedProcessData*> ManagedProcessAccessFactory::getManagedProcesses(std::string hostname)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

        // Get the snscon_id of the System Controller.
        std::ostringstream sql;
        sql << "SELECT snscon_id FROM sn_system_controller WHERE name = '"
            << databaseConnection->escapeQueryString(hostname) << "'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "key";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            char reason[256] = {0};
			sprintf(reason, "No System Controller found with name = %s",
                    hostname.c_str());			
            TA_THROW(DataException(reason, DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            char reason[256] = {0};
			sprintf(reason, "More than one System Controller found with name = %s",
                    hostname.c_str());			
            TA_THROW(DataException(reason, DataException::NOT_UNIQUE,""));
        }

        unsigned long systemControllerKey = data->getUnsignedLongData(0, keyColumn);
        delete data;
        data = NULL;

        // Get the entity keys of the managed processes the System Controller
        // must start.
        sql.str("");
        sql << "SELECT managed_process_pkey FROM sn_managed_process where "
            << "snscon_id = " << systemControllerKey;

        // Set up the columnNames vector to be passed to executeQuery()
        keyColumn = "snscon_id";
        columnNames.clear();
        columnNames.push_back(keyColumn);

        // Execute the query. 
        data = databaseConnection->executeQuery(sql.str(), columnNames);

        // Create a vector of IManagedProcessData pointers, and populate it.
        std::vector<IManagedProcessData*> result;
		do {
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				unsigned long key = data->getUnsignedLongData(i, keyColumn);
				result.push_back(new ManagedProcessData(systemControllerKey, key));
			}

			delete data;
			data = NULL;
		}
		while ( databaseConnection->moreData(data) );
        return result;
    }

    std::vector<std::string> ManagedProcessAccessFactory::getPeersOfSystemController(std::string hostname)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
    
        // Create the query.
        std::ostringstream sql;
        sql << "SELECT name FROM sn_system_controller WHERE snscon_id IN "
	        << "(SELECT UNIQUE snscon_id FROM sn_managed_process WHERE snscon_id <> "
            << "(SELECT snscon_id FROM sn_system_controller WHERE name = '"
            << databaseConnection->escapeQueryString(hostname) << "') and managed_process_pkey in "
	        << "(SELECT managed_process_pkey FROM sn_managed_process where snscon_id = "
	 	    << "(SELECT snscon_id FROM sn_system_controller WHERE name = '"
            << databaseConnection->escapeQueryString(hostname) << "')))";

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        // Create a vector of strings, and populate it.
        std::vector<std::string> result;
		do
		{		
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				result.push_back(data->getStringData(i, nameColumn));
			}

			delete data;
			data = NULL;
		}
		while ( databaseConnection->moreData(data) );
        return result;
    }


	void ManagedProcessAccessFactory::getAllPeersOfManagedProcess(ProcessToHostnameMap& processToHostnameMap)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
    
        // Create the query.
        std::ostringstream sql;

		sql << " select a.managed_process_pkey, b.name FROM sn_managed_process a,sn_system_controller b"
			<< " where a.snscon_id = b.snscon_id";

        // Set up the columnNames vector to be passed to executeQuery()
		std::string keyColumn  = "managed_process_pkey";
        std::string nameColumn = "name";
        std::vector<std::string> columnNames;
		columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        // Create a vector of strings, and populate it.
		do
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				unsigned long key = data->getUnsignedLongData(i, keyColumn);
				std::string name = data->getStringData(i, nameColumn);

				processToHostnameMap[key].push_back(name);
			}

			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
    }

	std::vector<std::string> ManagedProcessAccessFactory::getPeersOfManagedProcessBySql(IDatabase* databaseConnection, const std::string& sql)
    {
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Create a vector of strings, and populate it.
        std::vector<std::string> hostnames;
		do
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				hostnames.push_back(data->getStringData(i, columnNames[0]));
			}

			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

        return hostnames;
    }

	
	std::vector<std::string> ManagedProcessAccessFactory::getPeersOfManagedProcess(const unsigned long entityKey)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

        // Create the query.
        std::ostringstream sql;
        sql << "SELECT name FROM sn_system_controller WHERE snscon_id IN "
            << "(SELECT snscon_id FROM sn_managed_process WHERE managed_process_pkey = "
            << entityKey << ")";

		return getPeersOfManagedProcessBySql(databaseConnection, sql.str());
    }

    std::vector<std::string> ManagedProcessAccessFactory::getPeersOfManagedProcess(const std::string& entityName)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
    
        // Create the query.
        std::ostringstream sql;
        sql << "SELECT name FROM sn_system_controller WHERE snscon_id IN "
            << "(SELECT snscon_id FROM sn_managed_process WHERE managed_process_pkey = "
            << "(SELECT pkey FROM entity WHERE name = '" << databaseConnection->escapeQueryString(entityName) << "'))";

        return getPeersOfManagedProcessBySql(databaseConnection, sql.str());
    }

} // namespace TA_Base_Core
