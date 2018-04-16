/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfgHelper.cpp $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ 
  * 
  * 1: Corrected the TA_ASERT message in Reload()
  * 2: Added the validation sentence to check the length of the data in ReloadUsing()
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioBtnCfgHelper.h"

namespace   //anonymous
{
    static const int  INVALID_VALUE = -1;
    static const char INVALID_RESOURCE_TYPE = ' ';

    //Table Name
    static const std::string    BTNCFG_TBL("ra_btncfg");        

    //Column Names in the table
    static const std::string    PKEY_COL("PKEY");
    static const std::string    OPERATOR_PROFILE_COL("OPERATOR_PROFILE");
    static const std::string    IS_PROFILE_COL("IS_PROFILE");
    static const std::string    BUTTONREF_COL("BUTTONREF");
    static const std::string    RESOURCE_TYPE_COL("RESOURCE_TYPE");
    static const std::string    RESOURCE_ID_COL("RESOURCE_ID");

}

namespace TA_IRS_Core
{
    RadioBtnCfgHelper::RadioBtnCfgHelper()
        :m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_VALUE),
        m_operator_profile(INVALID_VALUE),
        m_isProfile(false),
        m_buttonRef(INVALID_VALUE),
        m_resourceType(INVALID_RESOURCE_TYPE),
        m_resourceId(INVALID_VALUE)        
    {    
        FUNCTION_ENTRY("RadioBtnCfgHelper Constructor");
        FUNCTION_EXIT;
    }

    RadioBtnCfgHelper::RadioBtnCfgHelper(const RadioBtnCfgHelper& helper)
        :m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_VALUE),
        m_operator_profile(INVALID_VALUE),
        m_isProfile(false),
        m_buttonRef(INVALID_VALUE),
        m_resourceType(INVALID_RESOURCE_TYPE),
        m_resourceId(INVALID_VALUE)        
    {
        FUNCTION_ENTRY("RadioBtnCfgHelper Copy Constructor");
        FUNCTION_EXIT;
    }
        
    RadioBtnCfgHelper::RadioBtnCfgHelper(const unsigned long key)
        :m_isNew(false),
        m_isValidData(false),
        m_key(key),
        m_operator_profile(INVALID_VALUE),
        m_isProfile(false),
        m_buttonRef(INVALID_VALUE),
        m_resourceType(INVALID_RESOURCE_TYPE),
        m_resourceId(INVALID_VALUE)        
    {
        FUNCTION_ENTRY("RadioBtnCfgHelper(const unsigned long key)");
        FUNCTION_EXIT;
    }

    RadioBtnCfgHelper::RadioBtnCfgHelper(const unsigned long row, TA_Base_Core::IData &data)
        :m_isNew(false),
        m_isValidData(false),
        m_key(INVALID_VALUE),
        m_operator_profile(INVALID_VALUE),
        m_isProfile(false),
        m_buttonRef(INVALID_VALUE),
        m_resourceType(INVALID_RESOURCE_TYPE),
        m_resourceId(INVALID_VALUE)        
    {
        FUNCTION_ENTRY("RadioBtnCfgHelper(const unsigned long row, TA_Base_Core::IData &data)");

        reloadUsing(row, data);

        FUNCTION_EXIT;    
    }


    RadioBtnCfgHelper::RadioBtnCfgHelper(bool isProfile, const unsigned long profileid, const unsigned long btnref,  const RadioResource&  resource)
        :m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_VALUE),
        m_operator_profile(profileid),
        m_isProfile(isProfile),
        m_buttonRef(btnref),
        m_resourceType(resource.type),
        m_resourceId(resource.id)        
    {
    }

    RadioBtnCfgHelper::~RadioBtnCfgHelper()
    {
        FUNCTION_ENTRY("~RadioBtnCfgHelper");
        FUNCTION_EXIT;
    }

    unsigned long RadioBtnCfgHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }

        FUNCTION_EXIT;        
        return m_key;
    }

    unsigned long RadioBtnCfgHelper::getOperatorProfile()
    {
        FUNCTION_ENTRY("getOperatorProfile");

        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_operator_profile;
    }

    bool RadioBtnCfgHelper::isProfile()
    {
        FUNCTION_ENTRY("isProfile");
        
        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }
        
        FUNCTION_EXIT;
        return m_isProfile;
    }

    unsigned long RadioBtnCfgHelper::getButtonRef()
    {
        FUNCTION_ENTRY("getButtonRef");

        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_buttonRef;
    }

    char RadioBtnCfgHelper::getResourceType()
    {
        FUNCTION_ENTRY("getResourceType");

        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceType;
    }

    unsigned long RadioBtnCfgHelper::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");

        if( m_isValidData == false &&
            m_isNew == false)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceId;
    }

    bool RadioBtnCfgHelper::isNew() const
    {
        FUNCTION_ENTRY("isNew");
        FUNCTION_EXIT;
        return m_isNew;
    }

    void RadioBtnCfgHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(!m_isNew, "Attempted to call invalidate() on a new RadioBtnCfgHelper");

        m_isValidData = false;

        FUNCTION_EXIT;    
    }

    void RadioBtnCfgHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio button config information has already been written to the database. It doesn't make
        // any sense to reload an Radio button config information that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio button configuration does not yet exist in the database. " 
            "Therefore it cannot be reloaded.");
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }

        // create the SQL string to retrieve the data of the Train based upon the key
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_SELECT_66003, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_SELECT_66003, m_key);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(OPERATOR_PROFILE_COL);
        columnNames.push_back(IS_PROFILE_COL);
        columnNames.push_back(BUTTONREF_COL);
        columnNames.push_back(RESOURCE_TYPE_COL);
        columnNames.push_back(RESOURCE_ID_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        if(data->getNumRows() == 0) 
        {
            //Radio Button Operator Configuration Helper record not found
            std::ostringstream reason;
            reason << "No record found for Radio Button Operator Configuration Helper with key = " << m_key << std::endl;
            
            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                TA_Base_Core::DataException::NO_VALUE, "Radio Button Operator Configuration Helper" ) );
        }
        
        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio Button Operator Configuration Helper records."); 
        
        reloadUsing(0, *data);

        FUNCTION_EXIT;
    }

    void RadioBtnCfgHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key               = data.getUnsignedLongData(row, PKEY_COL);
        m_operator_profile  = data.getUnsignedLongData(row, OPERATOR_PROFILE_COL);
        m_isProfile         = data.getUnsignedLongData(row, IS_PROFILE_COL);
        m_buttonRef         = data.getUnsignedLongData(row, BUTTONREF_COL);
		
		//Haipeng Jiang added the validation for the data array length
		if (data.getStringData(row, RESOURCE_TYPE_COL).size() > 0)
			m_resourceType      = data.getStringData(row, RESOURCE_TYPE_COL)[0];
		else
			TA_THROW(TA_Base_Core::DataException("The table BUTTONREF_COL does not contain any record the related to the query ", 
			TA_Base_Core::DataException::NO_VALUE, "There is no any button configuration information"));

        m_resourceId        = data.getUnsignedLongData(row, RESOURCE_ID_COL);
        
        // Need to record that we now have valid data
        m_isValidData = true;

        FUNCTION_EXIT;    
    }


    void RadioBtnCfgHelper::writeButtonData()
    {
        FUNCTION_ENTRY("writeButtonData");

        std::vector<std::string> fieldNames;

        // validate each field
        if (m_operator_profile == INVALID_VALUE)
        {
            fieldNames.push_back(OPERATOR_PROFILE_COL);
        }
        if (m_buttonRef == INVALID_VALUE)
        {
            fieldNames.push_back(BUTTONREF_COL);
        }
        if (m_resourceType == INVALID_RESOURCE_TYPE)
        {
            fieldNames.push_back(RESOURCE_TYPE_COL);
        }
        if (m_resourceId == INVALID_VALUE)
        {
            fieldNames.push_back(RESOURCE_ID_COL);
        }

        // throw exception if any of the field is invalid
        if (! fieldNames.empty())
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Radio Button data not fully specified. " \
                                                              "Radio Button cannot be written to database.",
                                                              fieldNames));
        }

        // update the database...
        if (m_isNew)
        {
            addNewButton();
        }
        else
        {
            modifyExistingButton();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    void RadioBtnCfgHelper::addNewButton()
    {
        FUNCTION_ENTRY("addNewButton");

        unsigned long nextAvailableKey = getNextSequenceNumber();

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }

        // can proceed with adding the new record
        {       
//			std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_INSERT_66005,getLocalDatabaseName(), nextAvailableKey, m_operator_profile,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_INSERT_66005,getLocalDatabaseName(), nextAvailableKey, m_operator_profile,
				m_isProfile, m_buttonRef, m_resourceType, m_resourceId);
            databaseConnection->executeModification( strSql);

            // update key info for this new record
            m_key = nextAvailableKey;
        }

        FUNCTION_EXIT;    
    }

    void RadioBtnCfgHelper::modifyExistingButton()
    {
        FUNCTION_ENTRY("modifyExistingButton");

        validateKeyExistence();
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }

 
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_UPDATE_66006, getLocalDatabaseName(), m_operator_profile, m_isProfile, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_UPDATE_66006, getLocalDatabaseName(), m_operator_profile, m_isProfile,
			m_buttonRef, m_resourceType, m_resourceId, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    void RadioBtnCfgHelper::validateKeyExistence()
    {

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_SELECT_66001, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_SELECT_66001, m_key);

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

    unsigned long RadioBtnCfgHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
       /* const std::string sql("select RABTNP_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_BTNCFG_SELECT_66007 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_BTNCFG_SELECT_66007 );

        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");

        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }

	std::string RadioBtnCfgHelper::getLocalDatabaseName()
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


