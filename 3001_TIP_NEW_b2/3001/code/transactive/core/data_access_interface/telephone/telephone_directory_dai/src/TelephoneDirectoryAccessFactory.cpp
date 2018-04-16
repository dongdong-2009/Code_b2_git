/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  */
#pragma warning(disable:4786)
#include <algorithm>
#include <ctime>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.h"

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;

namespace TA_IRS_Core
{
    TelephoneDirectoryAccessFactory* TelephoneDirectoryAccessFactory::m_instance = NULL;

    TelephoneDirectoryAccessFactory& TelephoneDirectoryAccessFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if (m_instance == NULL)
        {
            m_instance = new TelephoneDirectoryAccessFactory();
        }

        FUNCTION_EXIT;    
        return *m_instance;
    }


    void TelephoneDirectoryAccessFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");
        if (m_instance != NULL)
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
        FUNCTION_EXIT;
    }
    
    
    ITelephoneDirectory* TelephoneDirectoryAccessFactory::getTelephoneDirectory(
                                        const unsigned long idKey,
                                        const bool readWrite, 
                                        const TelephoneDirectoryItemStatus stateOfRecords)
    {
        FUNCTION_ENTRY("getTelephoneDirectory");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        // create the SQL string to retrieve the name of the TelephoneDirectory
        // this is a check to ensure an TelephoneDirectory with the specified
        // PKEY actually exists.
       /* std::ostringstream sql;
        sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY            << ","
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY    << ","
		    << TelephoneDirectoryHelper::DIR_TEL_NUM            << ","
            << TelephoneDirectoryHelper::DIR_FULL_NAME          << ","
            << TelephoneDirectoryHelper::DIR_LOCATION           << ","
		    << TelephoneDirectoryHelper::DIR_STATUS
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE
            << " where "
            << TelephoneDirectoryHelper::DIR_PRI_KEY << "=" << idKey
            << " and " 
            << TelephoneDirectoryHelper::DIR_STATUS << "=" << stateOfRecords;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57001, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57001, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57001,
			idKey, stateOfRecords);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(TelephoneDirectoryHelper::DIR_PRI_KEY);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_TEL_NUM);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_FULL_NAME);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_LOCATION);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_STATUS);


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No entry found for id = " << idKey;

            FUNCTION_EXIT;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "More than one entry found for id = " << idKey;

            FUNCTION_EXIT;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Create the TelephoneDirectory object to represent this object. 
        ITelephoneDirectory* theTelephoneDirectory;
        
        // decide which TelephoneDirectory object to return
        if (readWrite) // need a config TelephoneDirectory
        {
            theTelephoneDirectory = new ConfigTelephoneDirectory(0, *data);
        }
        else // need a standard TelephoneDirectory
        {
            theTelephoneDirectory = new TelephoneDirectory(0, *data);
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Return the TelephoneDirectory pointer. The class that recieves this pointer is responsible
        // for deleting it.
        FUNCTION_EXIT;
        return theTelephoneDirectory;
    }

    bool TelephoneDirectoryAccessFactory::isTelephoneDirectoryWithSubDir(  const unsigned long subDirKey,
                                                            const TelephoneDirectoryItemStatus stateOfRecords)
    {
        FUNCTION_ENTRY("isTelephoneDirectoryWithSubDir");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        // create the SQL string to retrieve the name of the TelephoneDirectory
        // this is a check to ensure an TelephoneDirectory with the specified
        // PKEY actually exists.
       /* std::ostringstream sql;
        sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE
            << " where "
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY << "=" << subDirKey
            << " and " 
            << TelephoneDirectoryHelper::DIR_STATUS << "=" << stateOfRecords;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57002, subDirKey, stateOfRecords);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57002, subDirKey, stateOfRecords);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57002, subDirKey, stateOfRecords);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(TelephoneDirectoryHelper::DIR_PRI_KEY);


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

        bool retval = true;
        
        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            retval = false;
        }     

        // clean up the pointer
        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return retval;
    }


    ITelephoneDirectories TelephoneDirectoryAccessFactory::getAllTelephoneDirectories(
                                        const bool readWrite, 
                                        const TelephoneDirectoryItemStatus stateOfRecords)
    {
        FUNCTION_ENTRY("getAllTelephoneDirectories");
    
        // create the SQL string to retrieve the data of the TelephoneDirectory
        // based upon the key
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
       /* std::ostringstream sql;
        sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY            << ","
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY    << ","
		    << TelephoneDirectoryHelper::DIR_TEL_NUM            << ","
            << TelephoneDirectoryHelper::DIR_FULL_NAME          << ","
            << TelephoneDirectoryHelper::DIR_LOCATION           << ","
		    << TelephoneDirectoryHelper::DIR_STATUS
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE
            << " where " 
            << TelephoneDirectoryHelper::DIR_STATUS << "=" << stateOfRecords;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57003, stateOfRecords);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57003, stateOfRecords);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57003, stateOfRecords);

        // Set up the columnNames vector to be passed to executeQuery()

        FUNCTION_EXIT;
        return getAllTelephoneDirectories(readWrite, strSql, databaseConnection);
    }
  

    ITelephoneDirectories TelephoneDirectoryAccessFactory::getAllStatusTelephoneDirectories(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllStatusTelephoneDirectories");

        // create the SQL string to retrieve the data of the TelephoneDirectory
        // based upon the key
		 TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
      /*  std::ostringstream sql;
        sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY            << ","
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY    << ","
		    << TelephoneDirectoryHelper::DIR_TEL_NUM            << ","
            << TelephoneDirectoryHelper::DIR_FULL_NAME          << ","
            << TelephoneDirectoryHelper::DIR_LOCATION           << ","
		    << TelephoneDirectoryHelper::DIR_STATUS
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57004);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57004);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57004);

        FUNCTION_EXIT;
        return getAllTelephoneDirectories(readWrite, strSql, databaseConnection);
    }


    ITelephoneDirectories TelephoneDirectoryAccessFactory::getAllTelephoneDirectories(const bool readWrite, const TA_Base_Core::SQLStatement& sql, TA_Base_Core::IDatabase* pDb)
    {
        FUNCTION_ENTRY("getAllTelephoneDirectories");

        // get a connection to the database
       /* TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);*/

        if (pDb == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(TelephoneDirectoryHelper::DIR_PRI_KEY);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_TEL_NUM);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_FULL_NAME);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_LOCATION);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_STATUS);


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = pDb->executeQuery(sql, columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");
       
        // create the vector of pointers to IEntityData
        ITelephoneDirectories dirList;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                if (readWrite)
                {
                    dirList.push_back(new ConfigTelephoneDirectory(i, *data));
                }
                else
                {
                    dirList.push_back(new TelephoneDirectory(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while( pDb->moreData(data) );

        FUNCTION_EXIT;
        return dirList;
    }

	//libo++
	ITelephoneDirectories TelephoneDirectoryAccessFactory::getTelephoneDirectoriesScope(const bool readWrite ,
                            const std::string& telephoneNumScope,
                            const TelephoneDirectoryItemStatus stateOfRecords)
	{
		FUNCTION_ENTRY("getTelephoneDirectoriesScope");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        // create the SQL string to retrieve the guis
        std::ostringstream sql;

       /* sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY            << ","
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY    << ","
		    << TelephoneDirectoryHelper::DIR_TEL_NUM            << ","
            << TelephoneDirectoryHelper::DIR_FULL_NAME          << ","
            << TelephoneDirectoryHelper::DIR_LOCATION           << ","
		    << TelephoneDirectoryHelper::DIR_TABLE              << "."
            << TelephoneDirectoryHelper::DIR_STATUS
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE;*/
		
        if (telephoneNumScope.empty() == false)
        {
            sql << " where "
                << TelephoneDirectoryHelper::DIR_TEL_NUM 
                << " in (" 
                << (databaseConnection->escapeInsertString(telephoneNumScope)).c_str()
                << ")"; 
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57005, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57005, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57005, sql.str());
        std::vector<std::string> columnNames;
        columnNames.push_back(TelephoneDirectoryHelper::DIR_PRI_KEY);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_TEL_NUM);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_FULL_NAME);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_LOCATION);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_STATUS);
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

        // create the vector of pointers to IEntityData
        ITelephoneDirectories dirList;
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                if (readWrite == true)
                {
                    dirList.push_back( new ConfigTelephoneDirectory(i, *data));
                }
                else
                {
                    dirList.push_back( new TelephoneDirectory(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
        return dirList;
	}
	//++libo
    

    ITelephoneDirectories TelephoneDirectoryAccessFactory::getTelephoneDirectoriesWhere( const bool readWrite,
                                                        const std::string& telephoneNum,
                                                        const std::string& fullName,
                                                        const std::string& dirname,
                                                        const TelephoneDirectoryItemStatus stateOfRecords)
    {
        FUNCTION_ENTRY("getTelephoneDirectoriesWhere");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        // create the SQL string to retrieve the guis
        std::ostringstream sql, osDirName;

      /*  sql << "select "
            << TelephoneDirectoryHelper::DIR_PRI_KEY            << ","
            << TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY    << ","
		    << TelephoneDirectoryHelper::DIR_TEL_NUM            << ","
            << TelephoneDirectoryHelper::DIR_FULL_NAME          << ","
            << TelephoneDirectoryHelper::DIR_LOCATION           << ","
		    << TelephoneDirectoryHelper::DIR_TABLE              << "."
            << TelephoneDirectoryHelper::DIR_STATUS
            << " from "
            << TelephoneDirectoryHelper::DIR_TABLE;*/

        if(dirname.empty() == false)
        {
            osDirName << " inner join te_subdirectory te_sd on te_directory_entry.TESUBD_ID = te_sd.TESUBD_ID" ;
				      //<< /*" using ("*/" "
                      //<< TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY;

        }

        /*sql << " where "
            << TelephoneDirectoryHelper::DIR_TABLE  << "."
            << TelephoneDirectoryHelper::DIR_STATUS << "=" << stateOfRecords;*/

        if(dirname.empty() == false)
        {
            sql << " and "
                << TelephoneSubDirectoryHelper::SUB_DIR_NAME 
                << " like '%" 
                << (databaseConnection->escapeInsertString(dirname)).c_str()
                << "%'";
        }
        
        if (telephoneNum.empty() == false)
        {
            sql << " and "
                << TelephoneDirectoryHelper::DIR_TEL_NUM 
                << " = '" 
                << (databaseConnection->escapeInsertString(telephoneNum)).c_str()
                << "'"; 
        }

        if(fullName.empty() == false)
        {
            std::string caps(fullName);
            std::transform(caps.begin(), caps.end(), caps.begin(), toupper);

            sql << " and UPPER("
                << TelephoneDirectoryHelper::DIR_FULL_NAME 
                << ") like '%" 
                << (databaseConnection->escapeInsertString(caps)).c_str()
                << "%'";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_DIRECTORY_ENTRY_STD_SELECT_57006,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_DIRECTORY_ENTRY_SELECT_57006,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_DIRECTORY_ENTRY_SELECT_57006,
			osDirName.str(), stateOfRecords, sql.str());

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(TelephoneDirectoryHelper::DIR_PRI_KEY);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_TEL_NUM);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_FULL_NAME);
        columnNames.push_back(TelephoneDirectoryHelper::DIR_LOCATION);
		columnNames.push_back(TelephoneDirectoryHelper::DIR_STATUS);


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

        // create the vector of pointers to IEntityData
        ITelephoneDirectories dirList;
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                if (readWrite == true)
                {
                    dirList.push_back( new ConfigTelephoneDirectory(i, *data));
                }
                else
                {
                    dirList.push_back( new TelephoneDirectory(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
        return dirList;
    }


    IConfigTelephoneDirectory* TelephoneDirectoryAccessFactory::createTelephoneDirectory()
    {            
        FUNCTION_ENTRY("createTelephoneDirectory");
        FUNCTION_EXIT;
        return new ConfigTelephoneDirectory();    
    }


    IConfigTelephoneDirectory* TelephoneDirectoryAccessFactory::copyTelephoneDirectory(const IConfigTelephoneDirectory* dirToCopy)
    {
        FUNCTION_ENTRY("copyTelephoneDirectory");

        TA_ASSERT(dirToCopy !=  NULL, "The TelephoneDirectory to copy was NULL");
        
        const ConfigTelephoneDirectory* cast = dynamic_cast<const ConfigTelephoneDirectory*>(dirToCopy);

        TA_ASSERT(cast != NULL, "TelephoneDirectory passed could not be converted into a ConfigTelephoneDirectory");

        FUNCTION_EXIT;
        return new ConfigTelephoneDirectory(*cast);

    }

    TelephoneDirectoryAccessFactory::TelephoneDirectoryAccessFactory()
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }
} // closes TA_Core


