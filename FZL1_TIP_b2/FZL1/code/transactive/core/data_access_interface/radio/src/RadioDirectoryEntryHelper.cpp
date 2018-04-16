/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntryHelper.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioDirectoryEntryHelper class.
  */

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntryHelper.h"

using namespace TA_Base_Core;

namespace TA_IRS_Core
{
    //
    // Constants
    //
    
    // Table name
    const std::string RadioDirectoryEntryHelper::RA_DIRECTORYENTRY_TBL   = "ra_directory_entry";

    // Column names in RA_TEXTMESSAGE_INBOX
    const std::string RadioDirectoryEntryHelper::PKEY_COL            = "PKEY";
    const std::string RadioDirectoryEntryHelper::NAME_COL        = "NAME";
    const std::string RadioDirectoryEntryHelper::OWNER_COL       = "OWNER";
    const std::string RadioDirectoryEntryHelper::CATEGORY_COL     = "CATEGORY";
    // Used for data validation
    const unsigned long  RadioDirectoryEntryHelper::INVALID_KEY     =  -1;

    //
    // Constructors and destructor
    //
    
    RadioDirectoryEntryHelper::RadioDirectoryEntryHelper()
    :   m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_owner(INVALID_KEY),
        m_category(INVALID_KEY),
        m_name()
    {
        FUNCTION_ENTRY("RadioDirectoryEntryHelper ctor()");
        FUNCTION_EXIT;
    }

    RadioDirectoryEntryHelper::RadioDirectoryEntryHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_owner(INVALID_KEY),
        m_category(INVALID_KEY),
        m_name()
    {
        FUNCTION_ENTRY("RadioDirectoryEntryHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
        reloadUsing(row, data);
        FUNCTION_EXIT;    
    }

    RadioDirectoryEntryHelper::RadioDirectoryEntryHelper(const RadioDirectoryEntryHelper& theRadioDirectoryEntryHelper)
    :   m_isNew(true),
        m_isValidData(false),
        m_key(theRadioDirectoryEntryHelper.m_key),
        m_owner(theRadioDirectoryEntryHelper.m_owner),
        m_category(theRadioDirectoryEntryHelper.m_category),
        m_name(theRadioDirectoryEntryHelper.m_name)
    {
        FUNCTION_ENTRY("RadioDirectoryEntryHelper copy ctor");
        FUNCTION_EXIT;
    }

    RadioDirectoryEntryHelper::RadioDirectoryEntryHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(key),
        m_owner(INVALID_KEY),
        m_category(INVALID_KEY),
        m_name()
    {
        FUNCTION_ENTRY("RadioDirectoryEntryHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioDirectoryEntryHelper::RadioDirectoryEntryHelper(const unsigned long owner, const unsigned long category, std::string name,bool bNewData)
    :   m_isNew(bNewData),
        m_isValidData(false),
        m_owner(owner),
        m_category(category),
        m_name(name)
    {
        FUNCTION_ENTRY("RadioDirectoryEntryHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioDirectoryEntryHelper::~RadioDirectoryEntryHelper()
    {
        FUNCTION_ENTRY("~RadioDirectoryEntryHelper");
        FUNCTION_EXIT;
    }

    //
    // RadioDirectoryEntryHelper methods
    //

    unsigned long RadioDirectoryEntryHelper::getOwner()
    {
        FUNCTION_ENTRY("getOwner");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_owner;
    }

    unsigned long RadioDirectoryEntryHelper::getCategory()
    {
        FUNCTION_ENTRY("getCategory");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_category;
    }

    void RadioDirectoryEntryHelper::writeDirectoryEntryData()
    {
        FUNCTION_ENTRY("writeDirectoryEntryData");

        std::vector<std::string> fieldNames;

        // validate each field
        if (m_key == INVALID_KEY)
        {
            fieldNames.push_back(PKEY_COL);
        }
        if (m_owner == INVALID_KEY)
        {
            fieldNames.push_back(OWNER_COL);
        }
        if (m_category == INVALID_KEY)
        {
            fieldNames.push_back(CATEGORY_COL);
        }
        if (m_name.empty())
        {
            fieldNames.push_back(NAME_COL);
        }

        // throw exception if any of the field is invalid
        if (! fieldNames.empty())
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Radio Directory data not fully specified. " \
                                                              "Radio Directory cannot be written to database.",
                                                              fieldNames));
        }

        // update the database...
		try 
		{
			if (m_isNew)
			{
				addNewDirectoryEntry();
			}
			else
			{
				modifyExistingDirectoryEntry();
			}
			// after writing the data, what we hold is now valid
			m_isValidData = true;
			m_isNew = false;
		}
		catch (...)
		{

			TA_THROW(TA_Base_Core::DataConfigurationException("Exception occur while modifying \ adding directory on dbase" \
															   "Radio Directory cannot be written to database.",
															   fieldNames));
		}
       

        FUNCTION_EXIT;
    }

    void RadioDirectoryEntryHelper::deleteDirectoryEntry()
    {
        FUNCTION_ENTRY("deleteDirectoryEntry");

        TA_ASSERT(! m_isNew, "This Radio Directory does not yet exist in the database and therefore cannot be deleted."); 
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_DELETE_67006, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_DELETE_67006, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    unsigned long RadioDirectoryEntryHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }

    std::string RadioDirectoryEntryHelper::getName()
    {
        FUNCTION_ENTRY("getName");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_name;
    }

    void RadioDirectoryEntryHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(! m_isNew, "Attempted to call invalidate() on a new Radio Directory.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    //
    // Private methods
    //

    void RadioDirectoryEntryHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio TextMessage has already been written to the database. It doesn't make
        // any sense to reload an Radio TextMessage that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio Directory does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the data of the radio TextMessage based upon the key
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_SELECT_67003, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_SELECT_67003, m_key);
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(NAME_COL);
		columnNames.push_back(OWNER_COL);
		columnNames.push_back(CATEGORY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // radio TextMessage record not found
            std::ostringstream reason;
            reason << "No record found for Radio Directory with key = " << m_key << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Key Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio Directory records."); 
                
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void RadioDirectoryEntryHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data.getUnsignedLongData(row, PKEY_COL);        
        m_owner = data.getUnsignedLongData(row, OWNER_COL);
        m_category = data.getUnsignedLongData(row, CATEGORY_COL);
        m_name = data.getStringData(row, NAME_COL);

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void RadioDirectoryEntryHelper::addNewDirectoryEntry()
    {
        FUNCTION_ENTRY("addNewDirectoryEntry");

        unsigned long nextAvailableKey = getNextSequenceNumber();
		
		if (-1 == nextAvailableKey)
		{
			TA_THROW( DataException("The Sequence is not found on the database", DataException::NO_VALUE,""));
		}

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        // can proceed with adding the new record
        {       
//			std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_INSERT_67007, nextAvailableKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_INSERT_67007, nextAvailableKey,
				m_owner, m_category, databaseConnection->escapeInsertString(m_name));
        
            databaseConnection->executeModification( strSql );

            // update key info for this new record
            m_key = nextAvailableKey;
        }

        FUNCTION_EXIT;    
    }

    void RadioDirectoryEntryHelper::modifyExistingDirectoryEntry()
    {
        FUNCTION_ENTRY("modifyExistingDirectoryEntry");

        validateKeyExistence();
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
    
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_UPDATE_67008, m_owner, m_category, m_name, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_UPDATE_67008, m_owner, m_category, m_name, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    void RadioDirectoryEntryHelper::validateKeyExistence()
    {
         // check if radio TextMessage id is in use
        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_ENTRY_SELECT_67004, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_ENTRY_SELECT_67004, m_key);
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (1 != data->getNumRows())
        {
            // cannot find our key in the db

            std::ostringstream reason;
			reason  << "A record with primary key " << m_key << " does not exist.";

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NOT_UNIQUE, "Primary Key"));
        }
    }

    unsigned long RadioDirectoryEntryHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
        /*const std::string sql("select RADENT_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_DIRECTORY_ENTRY_SELECT_67009 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_DIRECTORY_ENTRY_SELECT_67009 );
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
      
		if (data->getNumRows() != 1)
		{
			return -1;
		}

        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }
}
