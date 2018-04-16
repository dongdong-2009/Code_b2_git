/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMemberFactory.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng Jiang
  
  * 1: the namespace pulltion due to the const varaibles definition

  * Implementation of the Class RadioDirectoryMemberFactory
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/radio/src/RadioDirectoryMemberFactory.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMember.h"
#include "core/data_access_interface/radio/src/ConfigRadioDirectoryMember.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

using namespace TA_Base_Core;

namespace
{
    static const std::string TABLE_NAME = "ra_directory_member";
    static const std::string PARENT_COL = "PARENT";
    static const std::string TYPE_COL = "RESOURCE_TYPE";
    static const std::string ID_COL = "RESOURCE_ID";
}

namespace TA_IRS_Core
{

    RadioDirectoryMemberFactory* RadioDirectoryMemberFactory::m_instance = NULL;


    RadioDirectoryMemberFactory::RadioDirectoryMemberFactory()
    {
    }

    RadioDirectoryMemberFactory::~RadioDirectoryMemberFactory()
    {
    }

    RadioDirectoryMemberFactory& RadioDirectoryMemberFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");
		
		//using the thread lock
		static TA_Base_Core::ReadWriteThreadLockable lock;

		lock.acquire();

		//double check as below:
		if (NULL == m_instance)
		{
				
			if(NULL == m_instance)
			{
				m_instance = new RadioDirectoryMemberFactory();
			}
        }
		lock.release();

        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioDirectoryMemberFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    IRadioDirectoryMember* RadioDirectoryMemberFactory::getRadioDirectoryMember(unsigned long key, bool readWrite)
    {
        FUNCTION_ENTRY("getRadioDirectoryMember");
    
        TA_ASSERT(key > 0, "Invalid Radio Directory Id index.");

        // Create the radio directory object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioDirectoryMember* ret = NULL;
        
        // decide the type of radio directory object to return based on the readWrite param
        if (readWrite == true)
        {
            ret = new ConfigRadioDirectoryMember(key);
        }
        else
        {
            ret = new RadioDirectoryMember(key);
        }

        // Return the pointer. The caller is responsible for deleting it.
        FUNCTION_EXIT;
        return ret;
    }

    std::vector<IRadioDirectoryMember*> RadioDirectoryMemberFactory::getRadioDirectoryMembers(unsigned long parentKey)
    {
        FUNCTION_ENTRY("getRadioDirectoryMembers");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_SELECT_68002, parentKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_SELECT_68002, parentKey);
         
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PARENT_COL);
		columnNames.push_back(TYPE_COL);
        columnNames.push_back(ID_COL);       
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        std::vector<IRadioDirectoryMember*> rows;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                rows.push_back(new RadioDirectoryMember(i, *data));
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return rows;
    }

    IRadioDirectoryMember* RadioDirectoryMemberFactory::createNew(unsigned long parent, char resource_type, unsigned long resource_id)
    {
        ConfigRadioDirectoryMember* data = new ConfigRadioDirectoryMember(parent, resource_type, resource_id);
        data->save();

        return data;
    }

	void RadioDirectoryMemberFactory::deleteMembers(char resource_type, unsigned long resource_id)
	{
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        //delete by lin
        //std::ostringstream sql;
        //sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
		//	<< "delete " 
        //            << TABLE_NAME 
        //    << " where "
        //            << ID_COL  << " = " << resource_id << ", "  
        //            << TYPE_COL << " = '" << resource_type << "'" 
		//	<< "'); END;"
		//	<< std::ends;

       // databaseConnection->executeModification(sql.str().c_str());
        
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_UPDATE_68006,
			 getLocalDatabaseName(), resource_id, resource_type); 
		databaseConnection->executeModification(strSql);

	}

	void RadioDirectoryMemberFactory::updateMemberIds(char resource_type, unsigned long old_resource_id, unsigned long new_resource_id)
	{
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        //delete by lin
        //std::ostringstream sql;
        //sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
		//	<< "update " 
        //            << TABLE_NAME 
		//	<< " set " 
		//			<< ID_COL << " = " << new_resource_id 
        //    << " where "
        //            << ID_COL  << " = " << old_resource_id << ", "  
        //            << TYPE_COL << " = '" << resource_type << "'" 
		//	<< "'); END;"
		//	<< std::ends;

        //databaseConnection->executeModification(sql.str().c_str());
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_UPDATE_68007,
			getLocalDatabaseName(), new_resource_id, old_resource_id, resource_type);
		 
		databaseConnection->executeModification(strSql);

	}

	std::string RadioDirectoryMemberFactory::getLocalDatabaseName()
	{
		std::string localDatabaseName;    
		if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw TA_Base_Core::DatabaseException("db-connection-file not set");
		}
		try
		{
			TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
			localDatabaseName = connections[0].first;  
		}
		catch(...) // most likely a file not found error
		{
			throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

}

