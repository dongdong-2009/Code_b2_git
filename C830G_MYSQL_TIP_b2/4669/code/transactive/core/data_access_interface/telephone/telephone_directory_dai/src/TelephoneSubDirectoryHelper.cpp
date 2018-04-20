/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  * TelephoneSubDirectoryHelper is an object that is held by TelephoneSubDirectory and ConfigTelephoneSubDirectory objects.
  * It contains all data used by TE_SUBDIRECTORY, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneSubDirectory and ConfigTelephoneSubDirectory.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

using TA_Base_Core::DebugUtil;
using std::string;

namespace TA_IRS_Core
{
    const string TelephoneSubDirectoryHelper::SUB_DIR_SEQ       = "TESUBD_SEQ";
    const string TelephoneSubDirectoryHelper::SUB_DIR_TABLE     = "TE_SUBDIRECTORY";
    const string TelephoneSubDirectoryHelper::SUB_DIR_PRI_KEY   = "TESUBD_ID";
    const string TelephoneSubDirectoryHelper::SUB_DIR_NAME      = "NAME";
    const string TelephoneSubDirectoryHelper::SUB_DIR_STATUS    = "STATUS";

    TelephoneSubDirectoryHelper::TelephoneSubDirectoryHelper()
		: m_key(-1),
          m_subDirName(""),
          m_status(-1),
          m_isValidData(false),
          m_isNew(true)
    {
        FUNCTION_ENTRY("Default Constructor");
        FUNCTION_EXIT;
    }


    TelephoneSubDirectoryHelper::TelephoneSubDirectoryHelper(const unsigned long idKey)
		: m_key(idKey),
          m_subDirName(""),
          m_status(-1),
          m_isValidData(false),
          m_isNew(false)
    {
        FUNCTION_ENTRY("Constructor - idKey");
        FUNCTION_EXIT;
    }


	TelephoneSubDirectoryHelper::TelephoneSubDirectoryHelper( const TelephoneSubDirectoryHelper& theTelephoneSubDirectoryHelper)
		: m_key(-1),
          m_subDirName(theTelephoneSubDirectoryHelper.m_subDirName),
          m_status(theTelephoneSubDirectoryHelper.m_status),
          m_isValidData(false),
          m_isNew(true)
    {
        FUNCTION_ENTRY("Copy Constructor");
        FUNCTION_EXIT;
	}


    TelephoneSubDirectoryHelper::TelephoneSubDirectoryHelper(unsigned long row, TA_Base_Core::IData& data)
		: m_key(-1),
          m_subDirName(""),
          m_status(-1),
          m_isValidData(false),
          m_isNew(false)
    {
        FUNCTION_ENTRY("Constructor - IData");
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


    TelephoneSubDirectoryHelper::~TelephoneSubDirectoryHelper()
    {
        FUNCTION_ENTRY("Destructor");
        FUNCTION_EXIT;
    }


    long TelephoneSubDirectoryHelper::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }


    void TelephoneSubDirectoryHelper::setPrimaryKey(long idKey)
    {
        FUNCTION_ENTRY("setPrimaryKey" );
        TA_ASSERT(m_isNew,"An existing Sub Directory Key cannot be modified.");

        m_key = idKey;

        FUNCTION_EXIT;
        return;
    }


    std::string TelephoneSubDirectoryHelper::getSubDirName()
    {
        FUNCTION_ENTRY("getSubDirName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_subDirName;
    }


    void TelephoneSubDirectoryHelper::setSubDirName(const std::string& name)
    {
        FUNCTION_ENTRY("setSubDirName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_subDirName = name;
        FUNCTION_EXIT;
    }

    int TelephoneSubDirectoryHelper::getStatus()
    {
        FUNCTION_ENTRY("getStatus" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_status;
    }


    void TelephoneSubDirectoryHelper::setStatus(const int status)
    {
        FUNCTION_ENTRY("setStatus" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_status = status;
        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new Telephone Sub Directory");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the TelephoneSubDirectory has already been written to the database. It doesn't make
        // any sense to reload an TelephoneSubDirectory that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Telephone Sub Directory does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }


        // create the SQL string to retrieve the data of the ECSZone
        // based upon the key
       /* std::ostringstream sql;

        sql << "select "
            << SUB_DIR_NAME   << ","
            << SUB_DIR_STATUS << " from "
            << SUB_DIR_TABLE  << " where "
            << SUB_DIR_PRI_KEY<< "=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SUBDIRECTORY_STD_SELECT_57508, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_SELECT_57508, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_SELECT_57508, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(SUB_DIR_NAME);
		columnNames.push_back(SUB_DIR_STATUS);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            // ECS Zone record not found

            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Sub Directory with Key = " << m_key;
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Telephone Sub Directory Key" ) );
        }

        m_subDirName = data->getStringData( 0, SUB_DIR_NAME );
		m_status = data->getUnsignedLongData( 0, SUB_DIR_STATUS );

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data.getUnsignedLongData(row, SUB_DIR_PRI_KEY);
        m_subDirName = data.getStringData( row, SUB_DIR_NAME );
		m_status = data.getUnsignedLongData( row, SUB_DIR_STATUS );

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::writeTelephoneSubDirData()
    {
        FUNCTION_ENTRY( "writeTelephoneSubDirData" );

        if (m_isNew) // New Telephone Sub Directory
        {
            addNewTelephoneSubDir();
        }
        else // This is an existing Telephone Sub Directory
        {
            modifyExistingTelephoneSubDir();
        }

        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::deleteTelephoneSubDir(bool cascade)
    {
        FUNCTION_ENTRY("deleteTelephoneSubDir");

        TA_ASSERT(!m_isNew, "This Sub Directory does not yet exist in the database, and therefore cannot be deleted");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }
        //std::ostringstream sql;

        if (!cascade)
        {
            if(true == TelephoneDirectoryAccessFactory::getInstance().isTelephoneDirectoryWithSubDir(m_key))
            {
                TA_THROW(TA_Base_Core::DataException("A reference to this sub directory exists in the TE_DIRECTORY_ENTRY table",
                TA_Base_Core::DataException::CANNOT_DELETE,"TE_SUBDIRECTORY"));
            }
        }

        // Good to go...
       /* sql.str("");
        sql << "delete " << SUB_DIR_TABLE << " where " << SUB_DIR_PRI_KEY << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),TE_SUBDIRECTORY_STD_DELETE_57901, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_DELETE_57901, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_DELETE_57901, m_key);
        try
        {
            databaseConnection->executeModification(strSql);
        }
        catch(...)
        {
            TA_THROW(TA_Base_Core::DataException("Error executing SQL statement",TA_Base_Core::DataException::CANNOT_DELETE, "Telephone Directory Entry"));
        }

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::modifyExistingTelephoneSubDir()
    {
        FUNCTION_ENTRY("modifyExistingTelephoneSubDir");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }
        std::vector<std::string> fieldsNames;
        if (-1 == m_key)
        {
            fieldsNames.push_back(SUB_DIR_PRI_KEY);
        }
        if(m_subDirName.empty())
        {
            fieldsNames.push_back(SUB_DIR_NAME);
        }

        if(fieldsNames.empty() == false)
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Sub Directory data not completely specified. Data cannot be written to database", fieldsNames));
        }

        validateSubDirIdExistence();
        validateSubDirNameUniqueness();

        /*std::ostringstream sql;
        sql << "update " << SUB_DIR_TABLE << " set "
            << SUB_DIR_NAME << " = '" << (databaseConnection->escapeInsertString(m_subDirName)).c_str() << "', "
            << SUB_DIR_STATUS << " = " << m_status
            << " where " << SUB_DIR_PRI_KEY << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),TE_SUBDIRECTORY_STD_UPDATE_57651, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_UPDATE_57651, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_UPDATE_57651,
			databaseConnection->escapeInsertString(m_subDirName), m_status, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::addNewTelephoneSubDir()
    {
        FUNCTION_ENTRY("addNewTelephoneSubDir");

        std::vector<std::string> fieldsNames;
        if(m_subDirName.empty())
        {
            fieldsNames.push_back(SUB_DIR_NAME);
        }

        if(fieldsNames.empty() == false)
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Sub Directory Name is NULL. Data cannot be written to database", fieldsNames));
        }


        validateSubDirIdUniqueness();
        validateSubDirNameUniqueness();

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }
        m_key = getNextValidKey();

       /* std::ostringstream sql;
        sql << "insert into " << SUB_DIR_TABLE
            << "("
            << SUB_DIR_PRI_KEY << ","
            << SUB_DIR_NAME << ","
            << SUB_DIR_STATUS << ") values ("
            << m_key << ",'"
            << (databaseConnection->escapeInsertString(m_subDirName)).c_str() << "',"
            << m_status << ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),TE_SUBDIRECTORY_STD_INSERT_57801, m_key, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_INSERT_57801, m_key, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_INSERT_57801, m_key,
			databaseConnection->escapeInsertString(m_subDirName), m_status);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::validateSubDirIdExistence()
    {
        FUNCTION_ENTRY("validateSubDirIdExistence");

       /* std::ostringstream sql;
        sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE << " where " << SUB_DIR_PRI_KEY << " = " << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(SUB_DIR_PRI_KEY);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SUBDIRECTORY_STD_SELECT_57501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_SELECT_57501, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_SELECT_57501, m_key);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A sub directory of specified key " << m_key << " does not exist";
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"TE_SUBDIRECTORY Key"));
        }

        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }

    void TelephoneSubDirectoryHelper::validateSubDirIdUniqueness()
    {
        FUNCTION_ENTRY("validateSubDirIdUniqueness");

       /* std::ostringstream sql;
        sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE << " where " << SUB_DIR_PRI_KEY << " = " << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(SUB_DIR_PRI_KEY);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SUBDIRECTORY_STD_SELECT_57501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_SELECT_57501, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_SELECT_57501, m_key);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A sub directory exists with key =" << m_key;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"TE_SUBDIRECTORY Key"));
        }

        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void TelephoneSubDirectoryHelper::validateSubDirNameUniqueness()
    {
        FUNCTION_ENTRY("validateSubDirNameUniqueness");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }

//        std::ostringstream sql;
//        sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE
//            << " where " << SUB_DIR_NAME << " = '" << (databaseConnection->escapeInsertString(m_subDirName)).c_str() << "'"
//            << " and "   << SUB_DIR_STATUS << " = " << m_status;
//
//        if(m_isNew == false)
//        {
//
//            std::ostringstream sql;
//            sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE
//                << " where " << SUB_DIR_NAME << " = '" << (databaseConnection->escapeInsertString(m_subDirName)).c_str()
//                << "'";
//
//            // Set up the vector to pass to exectueQuery
//            std::vector<std::string> columnNames;
//            columnNames.push_back(SUB_DIR_PRI_KEY);
//
//            // execute the query
//            TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
//
//            if (0 != data->getNumRows())
//            {
//                delete data;
//                data = NULL;
//
//                std::ostringstream reasonMessage;
//    			reasonMessage << "A sub directory " << m_subDirName << " already exist. ";
//                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"TE_SUBDIRECTORY Key"));
//            }
//
//            delete data;
//            data = NULL;
//        }
		//std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
		
		if (m_isNew)
		{
			/*sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE
				<< " where " << SUB_DIR_NAME << " = '" << (databaseConnection->escapeInsertString(m_subDirName).c_str())
				<< "'";*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SUBDIRECTORY_STD_SELECT_57509, 
//			strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_SELECT_57509, 

		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_SELECT_57509,
				databaseConnection->escapeInsertString(m_subDirName));
		}
		else
		{
			/*sql << "select " << SUB_DIR_PRI_KEY << " from " << SUB_DIR_TABLE
				<< " where " << SUB_DIR_NAME << " = '" << (databaseConnection->escapeInsertString(m_subDirName).c_str()) << "'"
				<< " and " << SUB_DIR_STATUS << " = " << m_status;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SUBDIRECTORY_STD_SELECT_57510, 
//			strSql  = databaseConnection->prepareSQLStatement(TE_SUBDIRECTORY_SELECT_57510, 

		databaseConnection->prepareSQLStatement(strSql, TE_SUBDIRECTORY_SELECT_57510,
				databaseConnection->escapeInsertString(m_subDirName) ,m_status);
		}

		std::vector<std::string> columnNames;
		columnNames.push_back(SUB_DIR_PRI_KEY);

		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

		if (0 != data->getNumRows())
		{
			delete data;
			data = NULL;

			std::ostringstream reasonMessage;
			reasonMessage << "A subdirectory " << m_subDirName << " already exist. ";
			TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "TE_SUBDIRECTORY Key"));
		}

		delete data;
		data = NULL;

        FUNCTION_EXIT;
    }


    unsigned long TelephoneSubDirectoryHelper::getNextValidKey()
    {
        FUNCTION_ENTRY("getNextValidKey");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("Cannot connect to database"));
        }

        FUNCTION_EXIT;
        return DatabaseQueryHelper::getNextSequenceNumber(databaseConnection, SUB_DIR_SEQ);
    }
} // closes TA_Core
