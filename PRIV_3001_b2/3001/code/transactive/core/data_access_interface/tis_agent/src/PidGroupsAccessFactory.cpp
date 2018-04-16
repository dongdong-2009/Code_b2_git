/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * PidGroupsAccessFactory is a singleton that is used to generate and retrieve 
  * IPidGroup objects.
  */

#pragma warning ( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/PidGroup.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/exceptions/src/DataException.h"

#include "core/uuid/src/TAuuid.h"


namespace TA_IRS_Core
{

	TA_Base_Core::ReEntrantThreadLockable PidGroupsAccessFactory::m_singletonLock;
	PidGroupsAccessFactory* PidGroupsAccessFactory::m_theClassInstance = NULL;

	PidGroupsAccessFactory::PidGroupsAccessFactory()
		: m_isDirty(true), m_locationKey(0), m_locationSet(false), m_localDatabase("")
	{

	}

	PidGroupsAccessFactory::~PidGroupsAccessFactory()
	{
		//free memory and clear data 
		std::vector<IPidGroup*>::const_iterator it = m_groups.begin();
		for( ; it != m_groups.end(); it++) 
		{
			delete *it;
		}
		m_groups.clear();
	}

	PidGroupsAccessFactory& PidGroupsAccessFactory::getInstance( )
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			
            m_theClassInstance = new PidGroupsAccessFactory();
		}
		
		return *m_theClassInstance;
	}

	void PidGroupsAccessFactory::setLocation( unsigned long locationKey )
	{
		if(m_locationKey != locationKey)
		{
			m_isDirty = true;
		}

		m_locationSet = true;
		m_locationKey = locationKey;
	}

	std::vector<IPidGroup*> PidGroupsAccessFactory::getPidGroups()
	{
		
		if(m_isDirty)
		{
			updateData();
		}		

		return m_groups;
	}

	bool PidGroupsAccessFactory::pidGroupExists(const std::string& name)
	{
		//precondition checks:
		TA_ASSERT(!name.empty(), "!name.empty() failed");
		TA_ASSERT( m_locationSet, "m_locationSet failed");

		if(m_isDirty)
		{
			updateData();
		}

		//iterate through the groups and see if any have the name specified - if so return true
		std::vector<IPidGroup*>::iterator it = m_groups.begin();
		for( ; it != m_groups.end(); it++) 
		{
			if( name == (*it)->getName() )
			{
				return true;
			}
		}

		//no group with the specified name
		return false;
	}

	void PidGroupsAccessFactory::createPidGroup( 
		const std::string& name,
		std::vector<std::string> pidNames )
	{
		if( pidGroupExists( name ) ) //this call updates data if required and checks name
		{
			//does not send notification or update local data
			removeGroupFromTables( name );
		}

		//does not update local data
		addGroupToTables( name, pidNames );

		updateData();

	}




	void PidGroupsAccessFactory::deletePidGroup( const std::string& name )
	{
		removeGroupFromTables( name );

		updateData();
	}

	void PidGroupsAccessFactory::updateData()
	{

		//todo: lock methods so that we don't read while writing

		//free memory and clear current data 
		std::vector<IPidGroup*>::const_iterator it = m_groups.begin();
		for( ; it != m_groups.end(); it++) 
		{
			delete *it;
		}
		m_groups.clear();

		//read the table that defines the groups - but not the PIDs in the groups
		std::map< std::string, std::string > groupData = readGroupTable();

		//for each group you retrieved, read the PIDs in that group and add a PidGroup to m_groups
		std::map< std::string, std::string >::iterator groupIt = groupData.begin();
		for( ; groupIt != groupData.end(); groupIt++ )
		{
			//read the PIDs for this group
			std::vector< std::string > pidNames = readPidGroupMapTable( groupIt->second );

			m_groups.push_back(  new PidGroup( groupIt->first, pidNames) );
		}

		m_isDirty = false;
	}


    std::map< std::string, std::string > PidGroupsAccessFactory::readGroupTable()
    {
		
		unsigned long locationKey = getLocationKey();

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create select statement to get pkeys and names of groups at specified location:
       /* std::stringstream selectSql;
        selectSql << "select TIPGRO_ID, NAME from TI_PID_GROUPS where LOCATION_KEY = " << locationKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TI_PID_GROUPS_STD_SELECT_58501, locationKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PID_GROUPS_SELECT_58501, locationKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PID_GROUPS_SELECT_58501, locationKey);


        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( "TIPGRO_ID" );
		columnNames.push_back( "NAME" );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );
                
		std::map< std::string, std::string > groups;

		//if no data was returned there are no groups saved for this location so we're done
        if ( 0 == data->getNumRows() )
        {
            delete data;
			return groups;
        }

        // populate the map of group names/pkeys:
        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        groups.insert( std::pair< std::string, std::string> (
						data->getStringData( i, "NAME" ),
				        data->getStringData( i, "TIPGRO_ID" ) ) );
		        }
            
                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return groups;
    }

	std::vector< std::string > PidGroupsAccessFactory::readPidGroupMapTable( std::string groupKey )
	{

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create select statement to get PID names in specified group. This is done by joining the 
		//TI_PID_GROUP_MAP and ENTITY tables
       /* std::stringstream selectSql;
		selectSql << "SELECT ENTITY.NAME FROM ENTITY, TI_PID_GROUP_MAP ";
		selectSql << "where TI_PID_GROUP_MAP.TIPGRO_ID = '" <<  groupKey << "' ";
		selectSql << " and TI_PID_GROUP_MAP.PID_KEY = ENTITY.PKEY";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TI_PID_GROUPS_STD_SELECT_58502, groupKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PID_GROUPS_SELECT_58502, groupKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PID_GROUPS_SELECT_58502, groupKey);


        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
		columnNames.push_back( "NAME" );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );
                
		std::vector< std::string > pidNames;

		//if no data was returned there are no groups saved for this location so we're done
        if ( 0 == data->getNumRows() )
        {
            delete data;
			return pidNames;
        }

        // populate the vector of PID names
        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        pidNames.push_back( data->getStringData( i, "NAME" ) );
		        }
            
                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return pidNames;
	}

	unsigned long PidGroupsAccessFactory::getLocationKey()
	{

		TA_ASSERT( m_locationSet, "m_locationSet failed");
		return m_locationKey;
	}

	void PidGroupsAccessFactory::removeGroupFromTables( const std::string& name )
	{
		//precondition checks:
		TA_ASSERT(!name.empty(), "!name.empty() failed");
		unsigned long locationKey = getLocationKey(); //note that this tests location set precondition

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);


	/*	std::ostringstream sql;
		sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','DELETE', '";
		sql << "delete from TI_PID_GROUPS where NAME like ''" << name << "'' "
			<< " and LOCATION_KEY = " << locationKey;
		sql << "'); ";
		sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TI_PID_GROUPS_Oracle_DELETE_58951,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PID_GROUPS_DELETE_58951,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PID_GROUPS_DELETE_58951,
			getLocalDatabaseName(), name, locationKey);
		
		databaseConnection->executeModification( strSql );
	}

	void PidGroupsAccessFactory::addGroupToTables( const std::string& name, std::vector<std::string> pidNames )
	{
	   //precondition checks:
		TA_ASSERT(!name.empty(), "!name.empty() failed");
		unsigned long locationKey = getLocationKey(); //note that this tests location set precondition
		
		//will assert that none of the PID names are empty as well as ensuring that they are all datanodes
		std::vector<unsigned long> pidPkeys = getPidKeysFromNames( pidNames );

		//get a pkey to use for the new record in the TI_PID_GROUPS table
		std::string pkey = getNextPkeyForGroup();

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

	/*	std::ostringstream sql;
		sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '";
		sql << "insert into TI_PID_GROUPS(TIPGRO_ID,NAME,LOCATION_KEY) values (''" << pkey << "'', ''"
			<< databaseConnection->escapeAQSQLString(name) << "'', " << locationKey << ")"
			<< "'); ";
		sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TI_PID_GROUPS_Oracle_INSERT_58851,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PID_GROUPS_INSERT_58851,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PID_GROUPS_INSERT_58851,
			getLocalDatabaseName(), pkey, databaseConnection->escapeAQSQLString(name), locationKey);
		

		//add the PidGroup record
		databaseConnection->executeModification( strSql );


		//now add the pid records for the group to the database.
		//wrap all this in a try catch so we can back it out if we have to
		try
		{
			//one insert for each PID
			std::vector<unsigned long>::iterator it = pidPkeys.begin();
			for( ; it != pidPkeys.end(); it++) 
			{

			/*	std::ostringstream insertPidSql;
				insertPidSql << "BEGIN ";
				insertPidSql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '";
				insertPidSql << "insert into TI_PID_GROUP_MAP(PID_KEY,TIPGRO_ID) values (" << *it << ", ''" << pkey << "'')"
							 << "'); ";
				insertPidSql << "END;";*/
//				std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TI_PID_GROUPS_Oracle_INSERT_58852,
//				std::string strSql  = databaseConnection->prepareSQLStatement(TI_PID_GROUPS_INSERT_58852,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PID_GROUPS_INSERT_58852,
					getLocalDatabaseName(), *it, pkey);

				databaseConnection->executeModification( strSql );
			}
		}
		catch(...)
		{
			
			//back out any changes and report the failure
			removeGroupFromTables( name );

			throw;
		}		
	}

	std::vector<unsigned long> PidGroupsAccessFactory::getPidKeysFromNames( std::vector<std::string> pidNames )
	{
		//create the vector to be returned
		std::vector<unsigned long> pkeys;

		//Returns empty vector if pidNames is empty
		if(pidNames.size() == 0)
		{
			return pkeys;
		}

		//verify precondition that all elements are not empty while creating a list of the names
		//of the form ('<name1>', '<name2>'... '<nameN>')
		std::vector<std::string>::const_iterator it = pidNames.begin();
		std::stringstream nameStrm;
		nameStrm << "(";
		for( ; it != pidNames.end(); it++) 
		{
			TA_ASSERT(!it->empty(), "PID name passed in vector is empty");
			nameStrm << "'" << *it << "', ";
		}

		//remove the trailing ", " and add a trailing ")"
		std::string str = nameStrm.str();
		std::string names(str, 0, str.length() - 2);
		names += ")";        

		//example of valid query:
		//SELECT ENTITY.pkey FROM ENTITY, ENTITYTYPE WHERE ENTITY.name IN ('OCC.TIS.STIS.TIS02', 'OCC.TIS.STIS.TIS01', 'OCC.TIS.STIS.SEV01.aiiTISC-CurrentSTISLibraryVersion')
		//AND ENTITY.typekey = ENTITYTYPE.pkey
		//AND  ENTITYTYPE.name LIKE 'DataNode'

        // create select statement to PID keys from names:
      /*  std::stringstream selectSql;
		selectSql << "SELECT ENTITY.pkey FROM ENTITY, ENTITYTYPE WHERE ENTITY.name IN " << names;
		selectSql << " AND ENTITY.typekey = ENTITYTYPE.pkey";
		selectSql << " AND  ENTITYTYPE.name LIKE 'DataNode'";  */            

        std::vector< std::string > columnNames;
		columnNames.push_back( "pkey" );

		// Execute the query. The method can throw a TA_Base_Core::DatabaseException.
		// We are responsible for deleting the returned IData object when we're done with it		
		TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4514, names);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4514, names);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4514, names);
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );
                

		//if the number of rows returned does not match the number of elements in the original vector then
		//one or more of the entries was invalid so throw
        if ( pidNames.size() != data->getNumRows() )
        {
            delete data;
            std::stringstream error;
            error << "one or more of the PID names specified does not map to a entity of type DataNode";

			// TODO: this should be a DataException
            TA_THROW( TA_Base_Core::DatabaseException(error.str().c_str()) );
			
        }

        // populate the vector of PID pkeys
        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        pkeys.push_back( data->getUnsignedLongData( i, "pkey" ) );
		        }
            
                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return pkeys;
	}

	std::string PidGroupsAccessFactory::getNextPkeyForGroup()
	{
		return TA_Base_Core::TA_Uuid::generateUUID();
//		// run a query to update m_pkey
//        std::stringstream selectSql;
//        selectSql << "select TIPGRO_SEQ.NEXTVAL from DUAL";
//
//        std::vector< std::string > columnNames;
//        columnNames.push_back( "NEXTVAL" );
//
//		TA_Base_Core::IDatabase* databaseConnection = 
//            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
//        
//        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
//            databaseConnection->executeQuery( selectSql.str(), columnNames ) );
//
//        // Check that one entry was found
//        if ( 1 != data->getNumRows() )
//        {
//            std::stringstream error;
//            error << "Could not retrieve value from TIPGRO_SEQ.NEXTVAL";
//
//			// TODO: This should be a DataException
//            TA_THROW( TA_Base_Core::DatabaseException(error.str().c_str()) ); 
//        }
//
//        // get the pkey to use
//        return data->getUnsignedLongData( 0, "NEXTVAL" );
	}

	std::string PidGroupsAccessFactory::getLocalDatabaseName()
	{
		if (m_localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				TA_THROW(TA_Base_Core::DatabaseException("db-connection-file not set"));
			}
			try
			{
				/*TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
				m_localDatabase = connections[0].first;  */
				std::string strFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strFile, TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
			}
			catch(...) // most likely a file not found error
			{
				TA_THROW( TA_Base_Core::DatabaseException("Unable to find database connection infomation"));
			}
			
		}
		return m_localDatabase;
	}

} //end namespace TA_IRS_Core


