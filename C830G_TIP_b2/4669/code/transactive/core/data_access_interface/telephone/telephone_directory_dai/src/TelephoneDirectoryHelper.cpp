/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryHelper.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * TelephoneDirectoryHelper is an object that is held by TelephoneDirectoryEntry and ConfigTelephoneDirectoryEntry objects. 
  * It contains all data used by TelephoneDirectoryEntrys, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneDirectoryEntry and ConfigTelephoneDirectoryEntry.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryHelper.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryAccessFactory.h"

using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;
using std::string;

namespace TA_IRS_Core
{
    const string TelephoneDirectoryHelper::DIR_SEQ               = "TEDENT_SEQ";
    const string TelephoneDirectoryHelper::DIR_TABLE             = "TE_DIRECTORY_ENTRY";
    const string TelephoneDirectoryHelper::DIR_PRI_KEY           = "TEDENT_ID";
    const string TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY   = "TESUBD_ID";
    const string TelephoneDirectoryHelper::DIR_TEL_NUM           = "TEL_NUMBER";
    const string TelephoneDirectoryHelper::DIR_FULL_NAME         = "FULLNAME";
    const string TelephoneDirectoryHelper::DIR_LOCATION          = "LOCATION";
    const string TelephoneDirectoryHelper::DIR_STATUS            = "STATUS";

    
    TelephoneDirectoryHelper::TelephoneDirectoryHelper()
		: m_key(0),
		  m_subDirKey(0),
		  m_teleNum(""),
          m_fullName(""),
          m_location(""),
          m_status(0),
          m_isSubDirKeySet(false),
          m_isStatusSet(false),
          m_isValidData(false),
          m_isKeySet(false),
          m_isNew(true)
    {
    }

    TelephoneDirectoryHelper::TelephoneDirectoryHelper(const unsigned long idKey)
		: m_key(idKey),
		  m_subDirKey(0),
		  m_teleNum(""),
          m_fullName(""),
          m_location(""),
          m_status(0),
          m_isSubDirKeySet(false),
          m_isStatusSet(false),
          m_isValidData(false),
          m_isKeySet(true),
          m_isNew(false)
    {
    }


	TelephoneDirectoryHelper::TelephoneDirectoryHelper( const TelephoneDirectoryHelper& theTelephoneDirectoryHelper)
		: m_key(0),
		  m_subDirKey(theTelephoneDirectoryHelper.m_subDirKey),
		  m_teleNum(theTelephoneDirectoryHelper.m_teleNum),
          m_fullName(theTelephoneDirectoryHelper.m_fullName),
          m_location(theTelephoneDirectoryHelper.m_location),
          m_status(theTelephoneDirectoryHelper.m_status),
          m_isSubDirKeySet(true),
          m_isStatusSet(true),
          m_isValidData(false),
          m_isKeySet(false),
          m_isNew(true)
    {	  
	}



    TelephoneDirectoryHelper::TelephoneDirectoryHelper(unsigned long row, TA_Base_Core::IData& data)
		: m_key(0),
		  m_subDirKey(0),
		  m_teleNum(""),
          m_fullName(""),
          m_location(""),
          m_status(0),
          m_isSubDirKeySet(false),
          m_isStatusSet(false),
          m_isValidData(false),
          m_isKeySet(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    TelephoneDirectoryHelper::~TelephoneDirectoryHelper()
    {
    }


    

    unsigned long TelephoneDirectoryHelper::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }


    void TelephoneDirectoryHelper::setPrimaryKey(unsigned long idKey)
    {
        FUNCTION_ENTRY("setPrimaryKey" );
        TA_ASSERT(m_isNew,"An existing Telephone Directory cannot be modified.");

        m_key = idKey;
        m_isKeySet = true;

        FUNCTION_EXIT;
        return;
    }

    unsigned long TelephoneDirectoryHelper::getSubDirKey()
    {
        FUNCTION_ENTRY("getSubDirKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;

        return m_subDirKey;
    }


    void TelephoneDirectoryHelper::setSubDirKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setSubDirKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_subDirKey = entityKey;
        m_isSubDirKeySet = true;
        FUNCTION_EXIT;
    }

    std::string TelephoneDirectoryHelper::getTelephoneNum()
    {
        FUNCTION_ENTRY("getTelephoneNum" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_teleNum;
    }


    void TelephoneDirectoryHelper::setTelephoneNum(const std::string& number)
    {
        FUNCTION_ENTRY("setTelephoneNum" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_teleNum = number;
        FUNCTION_EXIT;
    }

    std::string TelephoneDirectoryHelper::getFullName()
    {
        FUNCTION_ENTRY("getFullName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_fullName;
    }


    void TelephoneDirectoryHelper::setFullName(const std::string& name)
    {
        FUNCTION_ENTRY("setFullName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_fullName = name;
        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::setLocation(const std::string& location)
    {
        FUNCTION_ENTRY("setLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_location = location;
        FUNCTION_EXIT;
    }


    std::string TelephoneDirectoryHelper::getLocation()
    {
        FUNCTION_ENTRY("getLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_location;
    }


    void TelephoneDirectoryHelper::setStatus(int status)
    {
        FUNCTION_ENTRY("setStnEquipmentEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_status = status;
        m_isStatusSet = true;

        FUNCTION_EXIT;
    }


    int TelephoneDirectoryHelper::getStatus()
    {
        FUNCTION_ENTRY("getIsIgnoringCongestion" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_status;
    }


    void TelephoneDirectoryHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new Telephone Directory");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void TelephoneDirectoryHelper::reload()
    {
        FUNCTION_ENTRY("reload" );

        // Ensure the TelephoneDirectoryEntry has already been written to the database. It doesn't make
        // any sense to reload an TelephoneDirectoryEntry that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Telephone Directory does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }


        // create the SQL string to retrieve the data of the Telephone Directory
        // based upon the key
        std::ostringstream sql;
        sql << "select "
            << DIR_SUB_DIR_PRI_KEY  <<","
            << DIR_TEL_NUM          <<","
            << DIR_FULL_NAME        <<","
            << DIR_LOCATION         <<","
            << DIR_STATUS
            << " from "
            << DIR_TABLE
            << " where "
            << DIR_PRI_KEY << "=" << m_key;

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(DIR_SUB_DIR_PRI_KEY);
		columnNames.push_back(DIR_TEL_NUM);
        columnNames.push_back(DIR_FULL_NAME);
        columnNames.push_back(DIR_LOCATION);
		columnNames.push_back(DIR_STATUS);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Teleohone Directory = " << m_key;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"Teleohone Directory key" ) );
        }
        
        if (1 < data->getNumRows()) 
        {
            // Multiple Records
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one record is found for Teleohone Directory with key = " << m_key;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"Teleohone Directory key" ) );
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_subDirKey = (long) data->getUnsignedLongData( 0, DIR_SUB_DIR_PRI_KEY);
        m_teleNum	= data->getStringData( 0, DIR_TEL_NUM);
		m_fullName = data->getStringData( 0, DIR_FULL_NAME);
		m_location  = data->getStringData( 0, DIR_LOCATION);
        m_status    = data->getIntegerData( 0, DIR_STATUS);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        m_isStatusSet = true;
        m_isSubDirKeySet = true;
        m_isKeySet = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key       = (long) data.getUnsignedLongData(row, DIR_PRI_KEY);
        m_subDirKey = (long) data.getUnsignedLongData(row, DIR_SUB_DIR_PRI_KEY);
        m_teleNum	= data.getStringData(row, DIR_TEL_NUM);
		m_fullName = data.getStringData(row, DIR_FULL_NAME);
		m_location  = data.getStringData(row, DIR_LOCATION);
        m_status    = data.getIntegerData(row, DIR_STATUS);

        // Need to record that we now have valid data
        m_isValidData = true;
        m_isSubDirKeySet = true;
        m_isKeySet = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::writeTelephoneDirectoryData()
        //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeTelephoneDirectoryData" );     

        // Write the TelephoneDirectoryEntry to the database

        // Need to check if this is a new TelephoneDirectoryEntry or an existing TelephoneDirectoryEntry - 
        // as a new TelephoneDirectoryEntry will be inserted, while an existing TelephoneDirectoryEntry will be updated
        if (m_isNew) // This is a new TelephoneDirectoryEntry
        {
            addNewTelephoneDirectoryData();
        }
        else // This is an existing TelephoneDirectoryEntry
        {
            modifyExistingTelephoneDirectoryData();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_isStatusSet = true;

        FUNCTION_EXIT;
    }


    void TelephoneDirectoryHelper::deleteTelephoneDirectoryData()
    {
        FUNCTION_ENTRY("deleteTelephoneDirectoryData");

        TA_ASSERT(!m_isNew, "This Telephone Directory does not yet exist in the database, and therefore cannot be deleted");
        

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        std::ostringstream sql;
        
        // Good to go...
        
        sql.str("");
        sql << "delete " << DIR_TABLE << " where " << DIR_PRI_KEY << "=" << m_key;
        try
        {
            databaseConnection->executeModification(sql.str());
        }
        catch(...)
        {
            TA_THROW(DataException("Error executing SQL statement", DataException::CANNOT_DELETE, "Telephone Directory Entry"));
        }

        FUNCTION_EXIT;
    }

    ITelephoneSubDirectory* TelephoneDirectoryHelper::getAssociatedSubdirectory()
    {
        FUNCTION_ENTRY("getAssociatedSubdirectory");
        FUNCTION_EXIT;
        return TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectory(m_subDirKey);
    } 		

    void TelephoneDirectoryHelper::modifyExistingTelephoneDirectoryData()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingTelephoneDirectoryData");

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (false == m_isKeySet)
        {
            fieldNames.push_back(DIR_PRI_KEY);
        }
        
        if (false == m_isSubDirKeySet)
        {
            fieldNames.push_back(DIR_SUB_DIR_PRI_KEY);
        }

        if (false == m_isStatusSet)
        {
            fieldNames.push_back(DIR_STATUS);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Directory Entry data not fully specified. Telephone Directory entry cannot be written to database",
                                                         fieldNames));
        }

        validateTelephoneDirectoryDataUniqueness();
        validateTelephoneSubDirectoryExistence(m_subDirKey);
        validateTelephoneNumberUniqueness();
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        std::ostringstream sql;
        sql << "update "
            << DIR_TABLE << " set "
            << DIR_SUB_DIR_PRI_KEY  << "=" << m_subDirKey << ","
            << DIR_TEL_NUM          << "='" << (databaseConnection->escapeInsertString(m_teleNum)).c_str() << "',"
            << DIR_FULL_NAME        << "='" << (databaseConnection->escapeInsertString(m_fullName)).c_str() << "',"
            << DIR_LOCATION         << "='" << (databaseConnection->escapeInsertString(m_location)).c_str() << "',"
            << DIR_STATUS           << "=" << m_status
            << " where "
            <<      DIR_PRI_KEY << "=" << m_key;


        databaseConnection->executeModification(sql.str().c_str());


        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void TelephoneDirectoryHelper::addNewTelephoneDirectoryData()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewTelephoneDirectoryData");

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (false == m_isSubDirKeySet)
        {
            fieldNames.push_back(DIR_SUB_DIR_PRI_KEY);
        }

        if (false == m_isStatusSet)
        {
            fieldNames.push_back(DIR_STATUS);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Directory Entry data not fully specified. Telephone Directory entry cannot be written to database",
                                                         fieldNames));
        }

        m_key = getNextValidKey();
        validateTelephoneDirectoryDataUniqueness();
        validateTelephoneSubDirectoryExistence(m_subDirKey);
        validateTelephoneNumberUniqueness();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

   
        std::ostringstream sql;
        sql << "insert into "
            << DIR_TABLE            << " ("
            << DIR_PRI_KEY          << ","
            << DIR_SUB_DIR_PRI_KEY  << ","
            << DIR_TEL_NUM          << ","
            << DIR_FULL_NAME        << ","
            << DIR_LOCATION         << ","
            << DIR_STATUS           
            << ") values ("
            << m_key << ","
            << m_subDirKey << ",'"
            << (databaseConnection->escapeInsertString(m_teleNum)).c_str()  << "','"
            << (databaseConnection->escapeInsertString(m_fullName)).c_str()<< "','"
            << (databaseConnection->escapeInsertString(m_location)).c_str() << "',"
            << m_status << ")" ;

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::validateTelephoneDirectoryDataUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("validateTelephoneDirectoryDataUniqueness");
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // First ensure the uniqueness of the record.
        //
        ////////////////////////////////////////////////////////////////////////////

        if (m_isNew)
        {
            std::ostringstream sql;
            sql << "select " << DIR_PRI_KEY << " from " << DIR_TABLE << " where " << DIR_PRI_KEY << " = " << m_key;

            // Set up the vector to pass to exectueQuery
            std::vector<std::string> columnNames;
            columnNames.push_back(DIR_PRI_KEY);

            // execute the query
            TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
            if (databaseConnection == NULL)
            {
                TA_THROW(DatabaseException("Cannot connect to database"));
            }

            TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
            if (0 != data->getNumRows())
            {
                // Telephone Directory record already exists
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A Telephone Directory record exists with Primary key = " << m_key;
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"Telephone Directory Key"));
            }

            delete data;
            data = NULL;

        }
        else
        { 
            // Do nothing
        }
        
        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::validateTelephoneSubDirectoryExistence(unsigned long entityKey)
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("validateTelephoneSubDirectoryExistence");
    
        std::ostringstream sql;
        sql << "select TESUBD_ID from TE_SUBDIRECTORY where TESUBD_ID = " << entityKey;
        
        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("TESUBD_ID");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // Status datapoint entitykey not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for entity key = " << entityKey;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"Telephone Subdirectory key" ) );
        }

        delete data;
        data = NULL;
        
        FUNCTION_EXIT;
    }

    void TelephoneDirectoryHelper::validateTelephoneNumberUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("validateTelephoneNumberUniqueness");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }
    
        // There is no need to check against itself
        std::ostringstream sql;
        sql << "select " << DIR_PRI_KEY << " from " << DIR_TABLE 
            << " where " << DIR_TEL_NUM << " = '" << (databaseConnection->escapeInsertString(m_teleNum)).c_str() << "'"
            << " and " << DIR_STATUS << " = " << m_status
            << " and " << DIR_PRI_KEY << " != " << m_key;

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(DIR_PRI_KEY);

        // execute the query

        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
    
        if (0 != data->getNumRows())
        {
            // Telephone Directory record already exists
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "Telephone Number ( " << m_teleNum << ") is not unique.";
            TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, DIR_TEL_NUM));
        }

        delete data;
        data = NULL;

        
        FUNCTION_EXIT;
    }


    unsigned long TelephoneDirectoryHelper::getNextValidKey()
    {
        FUNCTION_ENTRY("getNextValidKey");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        FUNCTION_EXIT;
        return DatabaseQueryHelper::getNextSequenceNumber(databaseConnection, DIR_SEQ);
    }
} // closes TA_Core
