/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriberHelper.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: Check the length of the array before using its element, pls see funcation ReloadUsing();
  * 2: changed the const varibles difinition
  * Implementation of the RadioSubscriberHelper class.
  */

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/radio/src/RadioSubscriberHelper.h"
#include "ace/OS.h"
#include <ctime>
#include <time.h>

namespace
{
   //
    // Constants
    //
    
    // Table name
    static const std::string RASUBSCRIBER_TBL   = "ra_subscriber";

    // Column names in RA_SUBSCRIBER
    static const std::string PKEY_COL           = "PKEY";
    static const std::string RESOURCETYPE_COL   = "RESOURCE_TYPE";
    static const std::string RESOURCEID_COL     = "RESOURCE_ID";
    static const std::string SSI_COL            = "SSI";
    static const std::string ALIAS_COL          = "ALIAS";
    static const std::string DATE_CREATED_COL   = "DATE_CREATED";   //TD17157
    static const std::string DATE_MODIFIED_COL  = "DATE_MODIFIED"; //TD17157
	static const std::string CREATED_BY_COL     = "CREATED_BY";
	static const std::string MODIFIED_BY_COL    = "MODIFIED_BY";
   
    static const char INVALID_CHAR  = ' ';
    // Used for data validation
    static const unsigned long  INVALID_KEY     =  -1;
	static const std::string PKEY_SEQ			= "RASUBS_SEQ.NEXTVAL";
}

using namespace TA_Base_Core;

namespace TA_IRS_Core
{
 


    //
    // Constructors and destructor
    //
    
    RadioSubscriberHelper::RadioSubscriberHelper()
    :   m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY),
        m_ssi(INVALID_KEY),
        m_alias(),
        m_dateCreated(0),
        m_dateModified(0)

    {
        FUNCTION_ENTRY("RadioSubscriberHelper ctor()");
        FUNCTION_EXIT;
    }

    RadioSubscriberHelper::RadioSubscriberHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY),
        m_ssi(INVALID_KEY),
        m_alias(),
        m_dateCreated(0),
        m_dateModified(0)
    {
        FUNCTION_ENTRY("RadioSubscriberHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
        reloadUsing(row, data);
        FUNCTION_EXIT;    
    }

    RadioSubscriberHelper::RadioSubscriberHelper(const RadioSubscriberHelper& theRadioSubscriberHelper)
    :   m_isNew(true),
        m_isValidData(false),
        m_key(theRadioSubscriberHelper.m_key),
        m_resourceType(theRadioSubscriberHelper.m_resourceType),
        m_resourceId(theRadioSubscriberHelper.m_resourceId),
        m_ssi(theRadioSubscriberHelper.m_ssi),
        m_alias(theRadioSubscriberHelper.m_alias),
        m_dateCreated(0),
        m_dateModified(0)
    {
        FUNCTION_ENTRY("RadioSubscriberHelper copy ctor");
        FUNCTION_EXIT;
    }

    RadioSubscriberHelper::RadioSubscriberHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(key),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY),
        m_ssi(INVALID_KEY),
        m_alias(),
        m_dateCreated(0),
        m_dateModified(0)
    {
        FUNCTION_ENTRY("RadioSubscriberHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioSubscriberHelper::RadioSubscriberHelper(const RadioResource&  resource)
    :   m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_resourceType(resource.type),
        m_resourceId(resource.id),
        m_ssi(resource.ssi),
        m_alias(resource.alias),
        m_dateCreated(0),
        m_dateModified(0)		
    {
        FUNCTION_ENTRY("RadioSubscriberHelper");
        if (!validateExistenceWithPkeyUnknown())
        {
            //if this entry exists already, should set the m_isNew to be false
            m_isNew = false;
        }
        FUNCTION_EXIT;
    }

    RadioSubscriberHelper::~RadioSubscriberHelper()
    {
        FUNCTION_ENTRY("~RadioSubscriberHelper");
        FUNCTION_EXIT;
    }

    //
    // RadioSubscriberHelper methods
    //

	char RadioSubscriberHelper::getResourceType()
    {
        FUNCTION_ENTRY("getResourceType");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceType;
    }
    
	unsigned long RadioSubscriberHelper::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceId;
    }

	unsigned long RadioSubscriberHelper::getSSI()
    {
        FUNCTION_ENTRY("getSSI");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ssi;
    }

    std::string RadioSubscriberHelper::getAlias()
    {
        FUNCTION_ENTRY("getAlias");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_alias;
    }

    void RadioSubscriberHelper::writeSubscriberData()
    {
        FUNCTION_ENTRY("writeSubscriberData");

        std::vector<std::string> fieldNames;

        // validate each field
        if (m_resourceType == INVALID_CHAR)
        {
            fieldNames.push_back(RESOURCETYPE_COL);
        }
        if (m_resourceId == INVALID_KEY)
        {
            fieldNames.push_back(RESOURCEID_COL);
        }
        if (m_ssi == INVALID_KEY)
        {
            fieldNames.push_back(SSI_COL);
        }
        if (m_alias.empty())
        {
            fieldNames.push_back(ALIAS_COL);            
        }

        // throw exception if any of the field is invalid
        if (! fieldNames.empty())
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Radio Subscriber data not fully specified. " \
                                                              "Radio Subscriber cannot be written to database.",
                                                              fieldNames));
        }

        // update the database...
        if (m_isNew)
        {
            addNewSubscriber();
        }
        else
        {		
            modifyExistingSubscriber();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void RadioSubscriberHelper::deleteSubscriber()
    {
        FUNCTION_ENTRY("deleteSubscriber");

        TA_ASSERT(! m_isNew, "This Radio Subscriber does not yet exist in the database and therefore cannot be deleted."); 

    /*    std::ostringstream sql;
        sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete " << RASUBSCRIBER_TBL << " where " << PKEY_COL << " = " << m_key
			<< "'); END;"
			<< std::ends; */

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_SUBSCRIBER_DELETE_70008, getLocalDatabaseName(), m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_DELETE_70008, getLocalDatabaseName(), m_key);
        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    unsigned long RadioSubscriberHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }

    void RadioSubscriberHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(! m_isNew, "Attempted to call invalidate() on a new Radio Subscriber.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    //
    // Private methods
    //

    void RadioSubscriberHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio Subscriber has already been written to the database. It doesn't make
        // any sense to reload an Radio Subscriber that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio Subscriber does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the data of the radio subscriber based upon the key
        /*std::ostringstream sql;
        sql << "select " 
                    << PKEY_COL << ", "
                    << RESOURCETYPE_COL << ", "
                    << RESOURCEID_COL << ", "
                    << SSI_COL << ", "
                    << ALIAS_COL << ", "
					<< "TO_CHAR(" << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
					<< "TO_CHAR(nvl(" << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
					<< " from " 
                    << RASUBSCRIBER_TBL << " where " 
                    << PKEY_COL << " = " << m_key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_SUBSCRIBER_SELECT_70005, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_SELECT_70005, m_key);
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(RESOURCETYPE_COL);
		columnNames.push_back(RESOURCEID_COL);
		columnNames.push_back(SSI_COL);
		columnNames.push_back(ALIAS_COL);
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL); 

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // radio subscriber record not found
            std::ostringstream reason;
            reason << "No record found for Radio Subscriber with key = " << m_key << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Key Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio Subscriber records."); 
                
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void RadioSubscriberHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data.getUnsignedLongData(row, PKEY_COL);   
		
		if (data.getStringData(row, RESOURCETYPE_COL).size() > 0)
			m_resourceType = data.getStringData(row, RESOURCETYPE_COL)[0];
		else
			TA_THROW(TA_Base_Core::DataException("The length of the Array is zero",
				TA_Base_Core::DataException::NO_VALUE, "Can not get the subscriber information from database" ) );
		
        m_resourceId = data.getUnsignedLongData(row,RESOURCEID_COL);
        m_ssi = data.getUnsignedLongData(row,SSI_COL);
        m_alias = data.getStringData(row,ALIAS_COL);

		m_dateCreated = data.getDateData( row, DATE_CREATED_COL );
        try
        {
            m_dateModified = data.getDateData( row, DATE_MODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,"DataException: %s",ex.what() );
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }
        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void RadioSubscriberHelper::addNewSubscriber()
    {
        FUNCTION_ENTRY("addNewSubscriber");

        unsigned long nextAvailableKey = 0;

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
		//TD17157 get a new pkey
       /* std::ostringstream sql;
        sql << "select " << PKEY_SEQ << " from DUAL";*/
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_SELECT_70006 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_SELECT_70006 );

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data->getNumRows() != 1)
        {
            TA_THROW(TA_Base_Core::DataException("Could not get primary key for new record",
                TA_Base_Core::DataException::MISSING_MANDATORY, PKEY_COL));
        }
        // get the pkey
        nextAvailableKey = data->getUnsignedLongData(0, "NEXTVAL");


		time_t now = time(NULL);
		char modifiedTimeString[24] = {0};
		struct tm modifiedTimeStruct;
        if (ACE_OS::localtime_r(&now, &modifiedTimeStruct) != NULL)
        {
	        sprintf(modifiedTimeString, "%02d/%02d/%04d %02d:%02d:%02d", modifiedTimeStruct.tm_mday, modifiedTimeStruct.tm_mon + 1, 
		        modifiedTimeStruct.tm_year + 1900, modifiedTimeStruct.tm_hour, modifiedTimeStruct.tm_min, modifiedTimeStruct.tm_sec);
        }
		std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

		//TD17157
        // can proceed with adding the new record
        {       
           /* std::ostringstream sql;
            sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
				<< "insert into " 
				<< RASUBSCRIBER_TBL 
				<< " (" 
				<< PKEY_COL << ", " 
				<< RESOURCETYPE_COL << ", "
				<< RESOURCEID_COL << ", "
				<< SSI_COL << ", "
				<< ALIAS_COL << ", "
				<< DATE_CREATED_COL << ", "
				<< CREATED_BY_COL << ") "
				<< "values ("
				<< nextAvailableKey << ", ''" 
				<< m_resourceType << "'', "
				<< m_resourceId << ", "
				<< m_ssi << ", ''"
				<< m_alias << "'',"
				<< "TO_TIMESTAMP(''" << std::string(modifiedTimeString) << "'',''dd/mm/yyyy hh24:mi:ss'')" << ", ''"
				<< agentName <<	"'')"
				<< "'); END;"
				<< std::ends;	*/
//			std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_INSERT_70009, getLocalDatabaseName(), nextAvailableKey, m_resourceType,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_INSERT_70009, getLocalDatabaseName(), nextAvailableKey, m_resourceType,
				m_resourceId, m_ssi, m_alias, std::string(modifiedTimeString),agentName);			
        
            databaseConnection->executeModification( strSql );

            // update key info for this new record
            m_key = nextAvailableKey;
        }

        FUNCTION_EXIT;    
    }

    void RadioSubscriberHelper::modifyExistingSubscriber()
    {
        FUNCTION_ENTRY("modifyExistingSubscriber");

        validateKeyExistence();

		time_t now = time(NULL);
		char modifiedTimeString[24] = {0};
		struct tm modifiedTimeStruct;
        if (ACE_OS::localtime_r(&now, &modifiedTimeStruct) != NULL)
        {
	        sprintf(modifiedTimeString, "%02d/%02d/%04d %02d:%02d:%02d", modifiedTimeStruct.tm_mday, modifiedTimeStruct.tm_mon + 1, 
		        modifiedTimeStruct.tm_year + 1900, modifiedTimeStruct.tm_hour, modifiedTimeStruct.tm_min, modifiedTimeStruct.tm_sec);
        }
		std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        
       /* std::ostringstream sql;
		sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<<"update " 
			<< RASUBSCRIBER_TBL //<< std::endl
			<< " set "
			<< RESOURCETYPE_COL  << " = ''" << m_resourceType << "'', "  //<< std::endl
			<< RESOURCEID_COL  << " = " << m_resourceId << ", " //<< std::endl
			<< SSI_COL  << " = " << m_ssi << ", " //<< std::endl
			<< ALIAS_COL  << " = ''" << m_alias << "''," //<< std::endl
			<< MODIFIED_BY_COL << " = ''" << agentName << "''," //<< std::endl
			<< DATE_MODIFIED_COL << "= TO_TIMESTAMP(''" << std::string(modifiedTimeString) << "'',''dd/mm/yyyy hh24:mi:ss'')" //<< std::endl
			<< " where " 
			<< PKEY_COL << " = " << m_key
			<< "'); END;"
			<< std::ends; */

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_UPDATE_70010, getLocalDatabaseName(), m_resourceType,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_UPDATE_70010, getLocalDatabaseName(), m_resourceType,
			m_resourceId, m_ssi, m_alias, agentName, std::string(modifiedTimeString), m_key);
        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    void RadioSubscriberHelper::validateKeyExistence()
    {
         // check if radio subscriber id is in use
       /* std::ostringstream sql;  
        sql << "select " 
                    << PKEY_COL << std::endl
            <<"from " 
                    << RASUBSCRIBER_TBL << std::endl
            << "where " 
                    << PKEY_COL << " = " << m_key << std::endl;*/

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_SELECT_70001, m_key );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_SELECT_70001, m_key );
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

    bool RadioSubscriberHelper::validateExistenceWithPkeyUnknown()
    {
         // check if radio subscriber id is in use
       /* std::ostringstream sql;  
        sql << "select " 
                    << PKEY_COL << std::endl
            <<"from " 
                    << RASUBSCRIBER_TBL << std::endl
            << "where " 
                    << RESOURCETYPE_COL << " = '" << m_resourceType << "' and "
                    << RESOURCEID_COL << " = " << m_resourceId << " and "
                    //<< SSI_COL << " = " << m_ssi << " and "
                    << ALIAS_COL << " = '" << m_alias << "'";*/

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_SELECT_70007, m_resourceType, m_resourceId, m_ssi,m_alias );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_SELECT_70007, m_resourceType, m_resourceId, m_ssi,m_alias );
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (1 == data->getNumRows())
        {
            m_key = data->getUnsignedLongData(0, PKEY_COL);
            return false;
        }
        else
        {
            return true;
        }
    }

    unsigned long RadioSubscriberHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
       /* const std::string sql("select RASUBS_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_SELECT_70006 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_SELECT_70006 );
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");

        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }

	void RadioSubscriberHelper::setResourceId(unsigned long rid)
	{
		m_resourceId = rid;
	}

	void RadioSubscriberHelper::setResourceType(RadioResourceType type)
	{
		m_resourceType = type;
	}

	void RadioSubscriberHelper::setSSI(unsigned long ssi)
	{
		m_ssi = ssi;
	}

	void RadioSubscriberHelper::setAlias(const std::string& alias)
	{
		m_alias = alias;
	}
	
	//TD17157
    time_t RadioSubscriberHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RadioSubscriberHelper::getDateModified");
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "!m_isValidData && !m_isNew");
            reload();
        }
        FUNCTION_EXIT;
        return m_dateModified;
    }

	time_t RadioSubscriberHelper::getDateCreated()
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		return m_dateCreated;		
	}
	//TD17157

	std::string RadioSubscriberHelper::getLocalDatabaseName()
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
