/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntryFactory.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ 
  *
  * 1: modified the namespace pollution to redefine the variables
  * 2: changed the number 4 to real column name "CATEGORY_COL" in funcation getRadioDirectoryEntries, pls see this SQL clause in "order by" part
  * Implementation of the Class RadioDirectoryEntryFactory
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioDirectoryEntryFactory.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntry.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h"
#include "core/data_access_interface/radio/src/ConfigRadioDirectoryEntry.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace
{
    //
    // Constants
    //    
    // Table name
    static const std::string RA_DIRECTORYENTRY_TBL   = "ra_directory_entry";
    static const std::string RA_DIRECTORYMEMBER_TBL  = "ra_directory_member";

    // Column names in DIRECTORY ENTRY
    static const std::string PKEY_COL        = "PKEY";
    static const std::string NAME_COL        = "NAME";
    static const std::string OWNER_COL       = "OWNER";
    static const std::string CATEGORY_COL    = "CATEGORY";

    // Column names in DIRECTORY MEMBER
    static const std::string PARENT_COL      = "PARENT";
    static const std::string RESOURCETYPE_COL= "RESOURCE_TYPE";
    static const std::string RESOURCEID_COL  = "RESOURCE_ID";
	
}

using namespace TA_Base_Core;

namespace TA_IRS_Core
{


    RadioDirectoryEntryFactory* RadioDirectoryEntryFactory::m_instance = NULL;


    RadioDirectoryEntryFactory::RadioDirectoryEntryFactory()
    {
    }

    RadioDirectoryEntryFactory::~RadioDirectoryEntryFactory()
    {
    }

    RadioDirectoryEntryFactory& RadioDirectoryEntryFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if(NULL == m_instance)
        {
            m_instance = new RadioDirectoryEntryFactory();
        }
        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioDirectoryEntryFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }


    std::vector<IRadioDirectoryEntry*> RadioDirectoryEntryFactory::getRadioDirectoryEntries(unsigned long category)
    {
        FUNCTION_ENTRY("getAllRadioDirectoryEntries");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        // create the SQL string to retrieve all the records from the table 
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_SELECT_67001,category, getOperatorId());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_SELECT_67001,category, getOperatorId());
         
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
		columnNames.push_back(NAME_COL);
        columnNames.push_back(OWNER_COL);       
        columnNames.push_back(CATEGORY_COL);       
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioDirectoryEntryHelper
        std::vector<IRadioDirectoryEntry*> directoryPtrVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);

                directoryPtrVector.push_back(new RadioDirectoryEntry(i, *data));
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return directoryPtrVector;
    }


    IRadioDirectoryEntry* RadioDirectoryEntryFactory::getRadioDirectoryEntry(unsigned long category, const std::string& name)
    {
        FUNCTION_ENTRY("getAllRadioDirectoryEntries");
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        // create the SQL string to retrieve all the records from the table 
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_SELECT_67002, category, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_SELECT_67002, category,
			getOperatorId(), databaseConnection->escapeQueryString(name));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(NAME_COL);
        columnNames.push_back(OWNER_COL);       
        columnNames.push_back(CATEGORY_COL);       
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data->getNumRows() != 1)
        {
            delete data;
            data = NULL;
            TA_THROW( DataException("Query does not return single row", DataException::NO_VALUE,""));
        }

        RadioDirectoryEntry* ret = new RadioDirectoryEntry(0, *data);

        delete data;
        data = NULL;
        return ret; // it's the caller responsibility to clean up
    }
    
   
	IRadioDirectoryEntry* RadioDirectoryEntryFactory::modifyDirectory(unsigned long category, std::string name)
	{

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve entry
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_SELECT_67002, category, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_SELECT_67002, category,
			getOperatorId(), databaseConnection->escapeQueryString(name));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(NAME_COL);
        columnNames.push_back(OWNER_COL);       
        columnNames.push_back(CATEGORY_COL);       
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

		IRadioDirectoryEntry* entry = NULL;
        if (data->getNumRows() == 0)
        {
			unsigned long operatorId = getOperatorId();	
			ConfigRadioDirectoryEntry* cfgData = new ConfigRadioDirectoryEntry(operatorId, category, name);
			cfgData->save();

			entry = cfgData;
        }
		else
		{
			RadioDirectoryEntry* ret = new RadioDirectoryEntry(0, *data);

			/*std::ostringstream sql;
			sql << "delete " << RA_DIRECTORYMEMBER_TBL << 
				" where " << PARENT_COL << " = " << ret->getKey();*/
//			std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_DELETE_68001, ret->getKey());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_DELETE_68001, ret->getKey());
				
			databaseConnection->executeModification( strSql );

			entry = ret;
		}

        delete data;
        data = NULL;

		return entry;
	}

	void RadioDirectoryEntryFactory::deleteDirectory(unsigned long category, std::string name)
	{
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }	

		IRadioDirectoryEntry* data = getRadioDirectoryEntry(category,name);

		if (data != NULL)
		{
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
//			strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_DELETE_68001, data->getKey());

		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_DELETE_68001, data->getKey());
				
			databaseConnection->executeModification(strSql);

//			strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_DELETE_67005, category,

		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_DELETE_67005, category,
				getOperatorId(), databaseConnection->escapeQueryString(name));

			databaseConnection->executeModification(strSql);
		}

	}

    unsigned long RadioDirectoryEntryFactory::getOperatorId()
    {
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        TA_Base_Core::ISession* session = NULL;
        unsigned long operatorId = 0;
        
        try
        {
            session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
            operatorId = session->getOperatorKey();
        }
        catch (...)
        {
            delete session;
            throw;
        }
        
        delete session;
        session = NULL;

        return operatorId;
    }
}

