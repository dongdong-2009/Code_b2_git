/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsTWPDataHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsTWPDataHelper is an object that is held by AtsTWPDataHelper and ConfigAtsTWPData objects. 
  * It contains all data used by AtsTWPDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsTWPDataHelper and ConfigAtsTWPData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsTWPDataHelper.h"
#include "core/data_access_interface/ats/src/AtsTWPDataFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string KEY_COL					= "ATTEMA_ID";
    static const std::string COMMAND_COL				= "COMMAND";
	static const std::string DATAPOINTENTITYKEY_COL		= "DATA_POINT_ENTITY_KEY";
	static const std::string DATAPOINTENTITYTYPE_COL	= "DATA_POINT_ENTITY_TYPE";
	
    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

	AtsTWPDataHelper::AtsTWPDataHelper( const AtsTWPDataHelper& theAtsTWPDataHelper)
		: m_idKey(-1),
		  m_twpDataPointEntityKey(theAtsTWPDataHelper.m_twpDataPointEntityKey),
		  m_twpDataPointType(theAtsTWPDataHelper.m_twpDataPointType),
		  m_twpCommand(theAtsTWPDataHelper.m_twpCommand),
          m_isValidData(false),
          m_isNew(true) 
    {	  
	}


    AtsTWPDataHelper::AtsTWPDataHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_twpDataPointEntityKey(-1),
		  m_twpDataPointType(""),
		  m_twpCommand(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsTWPDataHelper::AtsTWPDataHelper()
        : m_idKey(-1),
		  m_twpDataPointEntityKey(-1),
		  m_twpDataPointType(""),
		  m_twpCommand(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsTWPDataHelper::AtsTWPDataHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_twpDataPointEntityKey(-1),
		  m_twpDataPointType(""),
		  m_twpCommand(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsTWPDataHelper::~AtsTWPDataHelper()
    {
    }

	unsigned long AtsTWPDataHelper::getKey()
	{
		return m_idKey;
	}

	unsigned long AtsTWPDataHelper::getTWPDataPointEntityKey()
	{
        FUNCTION_ENTRY("getTWPDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_twpDataPointEntityKey;
    }

	std::string AtsTWPDataHelper::getTWPDataPointType()
	{
        FUNCTION_ENTRY("getTWPDataPointType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_twpDataPointType;
    }

	std::string AtsTWPDataHelper::getTWPCommand()
    {
        FUNCTION_ENTRY("getTWPCommand" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_twpCommand;
    }

    void AtsTWPDataHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsTWPDataHelper");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

	void AtsTWPDataHelper::setTWPDataPointEntityKey(unsigned long key)
	{
        FUNCTION_ENTRY("setTWPDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_twpDataPointEntityKey = key;
        FUNCTION_EXIT;
    }
		
	void AtsTWPDataHelper::setTWPDataPointType(std::string type)
	{
        FUNCTION_ENTRY("setTWPDataPointType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_twpDataPointType = type;
        FUNCTION_EXIT;
    }

	void AtsTWPDataHelper::setTWPCommand(std::string command)
	{
        FUNCTION_ENTRY("setTWPCommand" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_twpCommand = command;
        FUNCTION_EXIT;
    }


    void AtsTWPDataHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsTWPDataHelper has already been written to the database. It doesn't make
        // any sense to reload an AtsTWPDataHelper that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsTWPDataHelper does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsTWPDataHelper
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << COMMAND_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTENTITYTYPE_COL;
		sql << " from AT_TWP_ENTITY_MAP where ";
		sql << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_SELECT_50003, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_SELECT_50003, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_SELECT_50003, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COMMAND_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTENTITYTYPE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsTWPDataHelper record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PSD Entity Map Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"TWP Entity Map Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_twpCommand = data->getStringData(0, COMMAND_COL);
		m_twpDataPointEntityKey = data->getUnsignedLongData(0, DATAPOINTENTITYKEY_COL);
		m_twpDataPointType = data->getStringData(0, DATAPOINTENTITYTYPE_COL);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsTWPDataHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_twpCommand = data.getStringData(row, COMMAND_COL);
		m_twpDataPointEntityKey = data.getUnsignedLongData(row, DATAPOINTENTITYKEY_COL);
		m_twpDataPointType = data.getStringData(row, DATAPOINTENTITYTYPE_COL);

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsTWPDataHelper::writeAtsTWPDataData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsTWPDataData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		
		if (m_twpDataPointEntityKey == -1)
        {
            fieldNames.push_back(DATAPOINTENTITYKEY_COL);
        }
		if (m_twpCommand == "")
        {
            fieldNames.push_back(COMMAND_COL);
        }
		if (m_twpDataPointType == "")
        {
            fieldNames.push_back(DATAPOINTENTITYTYPE_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsTWPDataHelper data not fully specified. AtsTWPDataHelper cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsTWPDataHelper to the database

        // Need to check if this is a new AtsTWPDataHelper or an existing AtsTWPDataHelper - 
        // as a new AtsTWPDataHelper will be inserted, while an existing AtsTWPDataHelper will be updated
        if (m_isNew) // This is a new AtsTWPDataHelper
        {
            addNewAtsTWPData();
        }
        else // This is an existing AtsTWPDataHelper
        {
            modifyExistingAtsTWPData();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsTWPDataHelper::deleteAtsTWPData(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsTWPData");

        TA_ASSERT(!m_isNew, "This AtsTWPDataHelper does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
			// Nothing to check
        }

        // Good to go...
        
       /* sql.str("");
        sql << "delete AT_TWP_ENTITY_MAP where " << KEY_COL << " = " << m_idKey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_DELETE_50401, m_idKey);
//		strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_DELETE_50401, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_DELETE_50401, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AtsTWPDataHelper::modifyExistingAtsTWPData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsTWPData");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsTWPDataHelper id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

      /*  std::ostringstream sql;
        sql << "update AT_TWP_ENTITY_MAP set " 
			<< COMMAND_COL << " = '"
            << m_twpCommand << "', " 
			<< DATAPOINTENTITYKEY_COL << " = "
            << m_twpDataPointEntityKey << ", " 
			<< DATAPOINTENTITYTYPE_COL << " = '"
            << m_twpDataPointType << "' " 
            << " where " << KEY_COL << " = " << m_idKey;*/

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_UPDATE_50151, m_twpCommand,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_UPDATE_50151, m_twpCommand,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_UPDATE_50151, m_twpCommand,
			m_twpDataPointEntityKey, m_twpDataPointType, m_idKey);
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsTWPDataHelper::addNewAtsTWPData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsTWPData");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsTWPDataHelper %ul is a new AtsTWPDataHelper. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		/*std::ostringstream sql;
         sql << "insert into AT_TWP_ENTITY_MAP (" 
            << KEY_COL << ", "
			<< COMMAND_COL << ", " 
			<< DATAPOINTENTITYKEY_COL << ", " 
			<< DATAPOINTENTITYTYPE_COL << ") " 
            << "values ("
            << newKey << ", '" 
            << m_twpCommand << "', " 
			<< m_twpDataPointEntityKey << ", '" 
            << m_twpDataPointType << "')";*/
//		 std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_UPDATE_50301, newKey,
//		 std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_INSERT_50301, newKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_INSERT_50301, newKey,
			 m_twpCommand, m_twpDataPointEntityKey, m_twpDataPointType);

        databaseConnection->executeModification(strSql);

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsTWPDataHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		/*std::ostringstream sql;
        sql << "SELECT ATTEMA_SEQ.NEXTVAL FROM DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_Oracle_SELECT_50051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_SELECT_50051);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_SELECT_50051);

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");

        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        try 
        {
            // Should always be single row result
            TA_ASSERT(data->getNumRows() == 1, "Invalid result from getNextSequenceNumber");

            // Try and retrieve the value of auto increment ID
            unsigned long result = data->getUnsignedLongData(0, "NEXTVAL");

            // Clean up
            delete data;
            data = NULL;

            return result;
        }
        catch (TA_Base_Core::DataException&) 
        {
            // Clean up before rethrowing
            delete data;
            data = NULL;

            throw;
        }
	}

} // closes TA_Core

