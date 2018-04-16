/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInboxHelper.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $Haipeng Jiang
  * 
  * 1: added the codes to check the length of the array before using it, pls see the RaloadUsing() function 	
  * Implementation of the RadioTextMessageInboxHelper class.
  */

#include <sstream>
#include <iomanip>
#include "ace/OS.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxHelper.h"

namespace
{
    //
    // Constants
    //
    
    // Table name
    static const std::string RA_TEXTMESSAGEINBOX_TBL   = "ra_textmessage_inbox";

    // Column names in RA_TEXTMESSAGE_INBOX
    static const std::string PKEY_COL           = "PKEY";
    static const std::string TIMESTAMP_COL      = "TIMESTAMP";
    static const std::string OPERATOR_COL          = "OPERATOR";
    static const std::string ISREAD_COL   = "IS_READ";
    static const std::string MESSAGETEXT_COL     = "MESSAGETEXT";
	static const std::string SOURCEALIAS_COL     = "ALIAS";
//TD18825
	static const std::string RECIPIENTCONSOLE_COL = "RECIPIENTCONSOLE";
//TD18825   
    static const char ISREAD_YES     = 'Y';
    static const char ISREAD_NO      = 'N';

    // Used for data validation
    static const unsigned long  INVALID_KEY     =  -1;
}

using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Core
{
    void RadioTextMessageInboxHelper::getUpdateDateSql(TA_Base_Core::SQLTypeAdapter& rSqlParam, time_t datetime, bool bIsAQ)
    {
        FUNCTION_ENTRY("getUpdateDateSql");

        if (datetime == 0)
        {
            FUNCTION_EXIT;
            return ;
        }
	    
        struct tm dateStruct;
        std::ostringstream oStream;

        if (ACE_OS::localtime_r(&datetime, &dateStruct) != NULL)
        {
			//switch( nDbType )
			//{
			//case enumOracleDb:
			//	oStream << "TO_DATE('"
			//		<< setfill('0')
			//		<< std::setw(2) << dateStruct.tm_mday
			//		<< std::setw(2) << dateStruct.tm_mon + 1
			//		<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
			//		<< std::setw(2) << dateStruct.tm_hour
			//		<< std::setw(2) << dateStruct.tm_min 
			//		<< std::setw(2) << dateStruct.tm_sec
			//		<< "','DDMMYYYYHH24MISS')";
			//	break;
			//case enumMysqlDb:
			//	oStream << "STR_TO_DATE('"
			//		<< setfill('0')
			//		<< std::setw(2) << dateStruct.tm_mday
			//		<< std::setw(2) << dateStruct.tm_mon + 1
			//		<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
			//		<< std::setw(2) << dateStruct.tm_hour
			//		<< std::setw(2) << dateStruct.tm_min 
			//		<< std::setw(2) << dateStruct.tm_sec
			//		<< "','%d%m%Y%h%i%S')";
			//	break;
			//}	

			oStream << setfill('0')
				<< std::setw(2) << dateStruct.tm_mday
				<< std::setw(2) << dateStruct.tm_mon + 1
				<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
				<< std::setw(2) << dateStruct.tm_hour
				<< std::setw(2) << dateStruct.tm_min 
				<< std::setw(2) << dateStruct.tm_sec;

			rSqlParam.add(enumDateFormat_3, enumFuncFormat_2, oStream.str(), bIsAQ);                    
        }

        FUNCTION_EXIT;
        //return oStream.str();
    }
    //
    // Constructors and destructor
    //
    
    RadioTextMessageInboxHelper::RadioTextMessageInboxHelper()
    :   m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_timestamp(0),
        m_operator(INVALID_KEY),
        m_isread(false)
    {
        FUNCTION_ENTRY("RadioTextMessageInboxHelper ctor()");
        FUNCTION_EXIT;
    }

    RadioTextMessageInboxHelper::RadioTextMessageInboxHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_timestamp(0),
        m_operator(INVALID_KEY),
        m_isread(false)
    {
        FUNCTION_ENTRY("RadioTextMessageInboxHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
        reloadUsing(row, data);
        FUNCTION_EXIT;    
    }

    RadioTextMessageInboxHelper::RadioTextMessageInboxHelper(const RadioTextMessageInboxHelper& theRadioTextMessageInboxHelper)
    :   m_isNew(true),
        m_isValidData(false),
        m_key(theRadioTextMessageInboxHelper.m_key),
        m_timestamp(theRadioTextMessageInboxHelper.m_timestamp),
        m_operator(theRadioTextMessageInboxHelper.m_operator),
        m_isread(theRadioTextMessageInboxHelper.m_isread),
        m_messageText(theRadioTextMessageInboxHelper.m_messageText),
		m_recipientConsole(theRadioTextMessageInboxHelper.m_recipientConsole),
		m_sourceAlias(theRadioTextMessageInboxHelper.m_sourceAlias)
    {
        FUNCTION_ENTRY("RadioTextMessageInboxHelper copy ctor");
        FUNCTION_EXIT;
    }

    RadioTextMessageInboxHelper::RadioTextMessageInboxHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(key),
        m_timestamp(0),
        m_operator(INVALID_KEY),
        m_isread(false)
    {
        FUNCTION_ENTRY("RadioTextMessageInboxHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioTextMessageInboxHelper::RadioTextMessageInboxHelper(
		const time_t timestamp,const unsigned long oper,bool isread,const std::string& msgtext,const std::string& source, const std::string& recipient)
    :   m_isNew(false),
        m_isValidData(false),
        m_timestamp(timestamp),
        m_operator(oper),
        m_isread(isread),
        m_messageText(msgtext),
		m_sourceAlias(source),
		m_recipientConsole(recipient)
    {
        FUNCTION_ENTRY("RadioTextMessageInboxHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }


    RadioTextMessageInboxHelper::~RadioTextMessageInboxHelper()
    {
        FUNCTION_ENTRY("~RadioTextMessageInboxHelper");
        FUNCTION_EXIT;
    }

    //
    // RadioTextMessageInboxHelper methods
    //
    time_t RadioTextMessageInboxHelper::getTimeStamp()
    {
        FUNCTION_ENTRY("getTimeStamp");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_timestamp;
    }

    unsigned long RadioTextMessageInboxHelper::getOperator()
    {
        FUNCTION_ENTRY("getOperator");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_operator;
    }

    bool RadioTextMessageInboxHelper::isRead()
    {
        FUNCTION_ENTRY("isRead");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_isread;
    }

    std::string RadioTextMessageInboxHelper::getMessageText()
    {
        FUNCTION_ENTRY("getMessageText");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_messageText;
    }

    std::string RadioTextMessageInboxHelper::getSourceAlias()
    {
        FUNCTION_ENTRY("getMessageText");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_sourceAlias;
    }

	std::string RadioTextMessageInboxHelper::getRecipientConsole()
	{
        FUNCTION_ENTRY("getRecipientConsole");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;

        return m_recipientConsole;
	}

    void RadioTextMessageInboxHelper::setOperator(unsigned long operatorId)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_operator = operatorId;
    }

    void RadioTextMessageInboxHelper::setTimeStamp(time_t pTime)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_timestamp = pTime;
    }

    void RadioTextMessageInboxHelper::setRead(bool pRead)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isread = pRead;
    }

    void RadioTextMessageInboxHelper::setMessageText(const std::string& messageText)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_messageText = messageText;
    }

    void RadioTextMessageInboxHelper::setSourceAlias(const std::string& srcAlias)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_sourceAlias = srcAlias;
    }

	void RadioTextMessageInboxHelper::setRecipientConsole(const std::string& recipient)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		m_recipientConsole = recipient;

	}
    void RadioTextMessageInboxHelper::writeTextMessageData()
    {
        FUNCTION_ENTRY("writeTextMessageData");

        // update the database...
        if (m_isNew)
        {
            addNewTextMessage();
        }
        else
        {
            modifyExistingTextMessage();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    void RadioTextMessageInboxHelper::deleteTextMessage()
    {
        FUNCTION_ENTRY("deleteTextMessage");

        TA_ASSERT(! m_isNew, "This Radio TextMessageInbox does not yet exist in the database and therefore cannot be deleted."); 

       /* std::ostringstream sql;
        sql << "delete " << RA_TEXTMESSAGEINBOX_TBL << " where " << PKEY_COL << " = " << m_key;*/

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_TEXTMESSAGE_INBOX_DELETE_71005, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_TEXTMESSAGE_INBOX_DELETE_71005, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    unsigned long RadioTextMessageInboxHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }

    void RadioTextMessageInboxHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(! m_isNew, "Attempted to call invalidate() on a new Radio TextMessageInbox.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    //
    // Private methods
    //

    void RadioTextMessageInboxHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio TextMessage has already been written to the database. It doesn't make
        // any sense to reload an Radio TextMessage that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio TextMessageInbox does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the data of the radio TextMessage based upon the key
      /*  std::ostringstream sql;
        sql << "select " 
                    << PKEY_COL << ", "
                    << "to_char("<< TIMESTAMP_COL << ", 'YYYYMMDDHH24MISS'), "
                    << OPERATOR_COL << ", "
                    << ISREAD_COL << ", "
					<< MESSAGETEXT_COL << ", "
                    << SOURCEALIAS_COL << ", "
					<< RECIPIENTCONSOLE_COL << " from " 
                    << RA_TEXTMESSAGEINBOX_TBL << " where " 
                    << PKEY_COL << " = " << m_key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_TEXTMESSAGE_INBOX_SELECT_71003, m_key );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_TEXTMESSAGE_INBOX_SELECT_71003, m_key );
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(TIMESTAMP_COL);
        columnNames.push_back(OPERATOR_COL);
		columnNames.push_back(ISREAD_COL);
		columnNames.push_back(MESSAGETEXT_COL);
		columnNames.push_back(SOURCEALIAS_COL);
		columnNames.push_back(RECIPIENTCONSOLE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // radio TextMessage record not found
            std::ostringstream reason;
            reason << "No record found for Radio TextMessageInbox with key = " << m_key << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Key Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio TextMessageInbox records."); 
                
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void RadioTextMessageInboxHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data.getUnsignedLongData(row, PKEY_COL);        
        m_timestamp = data.getDateData(row,TIMESTAMP_COL);
        m_operator = data.getUnsignedLongData(row, OPERATOR_COL);
		
		if (data.getStringData(row, ISREAD_COL).size() > 0) 
			m_isread = data.getStringData(row, ISREAD_COL)[0] == ISREAD_YES;
		else
			TA_THROW(TA_Base_Core::DataException("The length of the array is zero",
			TA_Base_Core::DataException::NO_VALUE, "Can not get the text message inbox information" ));

        m_messageText = data.getStringData(row, MESSAGETEXT_COL);
		m_sourceAlias = data.getStringData(row, SOURCEALIAS_COL);
		m_recipientConsole = data.getStringData(row, RECIPIENTCONSOLE_COL);

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void RadioTextMessageInboxHelper::addNewTextMessage()
    {
        FUNCTION_ENTRY("addNewTextMessage");

        unsigned long nextAvailableKey = getNextSequenceNumber();

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        // can proceed with adding the new record
        {       
           /* std::ostringstream sql;
            sql << "insert into " 
                        << RA_TEXTMESSAGEINBOX_TBL 
                        << " (" 
                            << PKEY_COL << ", " 
                            << TIMESTAMP_COL << ", "
                            << OPERATOR_COL << ", "
                            << ISREAD_COL << ", "
							<< MESSAGETEXT_COL << ", "
							<< RECIPIENTCONSOLE_COL << ", "
                            << SOURCEALIAS_COL << ") "
                << "values ("
                        << nextAvailableKey << ", " 
                        << getUpdateDateSql(m_timestamp) << ", "
                        << m_operator << ", '"
                        << (m_isread ? ISREAD_YES : ISREAD_NO ) << "', '"
						<< databaseConnection->escapeInsertString(m_messageText) << "', '"
						<< databaseConnection->escapeInsertString(m_recipientConsole) << "', '"
                        << databaseConnection->escapeInsertString(m_sourceAlias) << "')";*/

			//int nDbType = databaseConnection->getDbServerType();
//			std::string strSql = databaseConnection->prepareSQLStatement(RA_TEXTMESSAGE_INBOX_INSERT_71006, nextAvailableKey, getUpdateDateSql(m_timestamp, nDbType),
			TA_Base_Core::SQLTypeAdapter sqlParam;
			getUpdateDateSql(sqlParam, m_timestamp, false);

			TA_Base_Core::SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, RA_TEXTMESSAGE_INBOX_INSERT_71006, nextAvailableKey, sqlParam,
				m_operator, (m_isread ? ISREAD_YES : ISREAD_NO ), databaseConnection->escapeInsertString(m_messageText),
				databaseConnection->escapeInsertString(m_recipientConsole), databaseConnection->escapeInsertString(m_sourceAlias));

        
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "SQL => %s", strSql.c_str());
			
			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case TA_Base_Core::enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case TA_Base_Core::enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "SQL => %s", strExSql.c_str());

            databaseConnection->executeModification( strSql );

            // update key info for this new record
            m_key = nextAvailableKey;
        }

        FUNCTION_EXIT;    
    }

    void RadioTextMessageInboxHelper::modifyExistingTextMessage()
    {
        FUNCTION_ENTRY("modifyExistingTextMessage");

        validateKeyExistence();
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
       /* std::ostringstream sql;
        sql << "update " 
                    << RA_TEXTMESSAGEINBOX_TBL << std::endl
            << "set "
                    << TIMESTAMP_COL << " = " << getUpdateDateSql(m_timestamp) << ", " << std::endl
                    << OPERATOR_COL  << " = " << m_operator << ", " << std::endl
                    << ISREAD_COL  << " = '" << (m_isread ? ISREAD_YES : ISREAD_NO ) << "', " << std::endl
					<< MESSAGETEXT_COL  << " = '" <<m_messageText << "', " << std::endl
					<< RECIPIENTCONSOLE_COL  << " = '" <<m_recipientConsole << "', " << std::endl
                    << SOURCEALIAS_COL  << " = '" << m_sourceAlias << "'" << std::endl
            << "where " 
                    << PKEY_COL << " = " << m_key;*/

		//int nDbType = databaseConnection->getDbServerType();
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_TEXTMESSAGE_INBOX_UPDATE_71007, getUpdateDateSql(m_timestamp, nDbType),
		TA_Base_Core::SQLTypeAdapter sqlParam;
		getUpdateDateSql(sqlParam, m_timestamp, false);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_TEXTMESSAGE_INBOX_UPDATE_71007, sqlParam,
			m_operator, (m_isread ? ISREAD_YES : ISREAD_NO ), m_messageText,m_recipientConsole, m_sourceAlias, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    void RadioTextMessageInboxHelper::validateKeyExistence()
    {
         // check if radio TextMessage id is in use
       /* std::ostringstream sql;  
        sql << "select " 
                    << PKEY_COL << std::endl
            <<"from " 
                    << RA_TEXTMESSAGEINBOX_TBL << std::endl
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
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_TEXTMESSAGE_INBOX_SELECT_71001, m_key );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_TEXTMESSAGE_INBOX_SELECT_71001, m_key );
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

    unsigned long RadioTextMessageInboxHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
        /*const std::string sql("select RATINB_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_TEXTMESSAGE_INBOX_SELECT_71004 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_TEXTMESSAGE_INBOX_SELECT_71004 );
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");

        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }
}
