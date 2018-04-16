/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioPredefinedTextMessageHelper.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioPredefinedTextMessageHelper class.
  */

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageHelper.h"

namespace
{
    //
    // Constants
    //
    
    // Table name
    static const std::string RA_PREDEFINEDTEXTMESSAGE_TBL   = "ra_predefined_textmessage";

    // Column names in RA_TEXTMESSAGE_INBOX
    static const std::string PKEY_COL            = "PKEY";
    static const std::string OPERATOR_COL        = "OPERATOR";
    static const std::string SHORTNAME_COL       = "SHORT_NAME";
    static const std::string MESSAGETEXT_COL     = "MESSAGETEXT";
	static const std::string TYPE_COL			 = "TYPE";
    // Used for data validation
    static const unsigned long  INVALID_KEY     =  -1;
}


using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Core
{


    //
    // Constructors and destructor
    //
    
    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper()
    :   m_isNew(true),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_operator(INVALID_KEY),
        m_shortName(),
		m_bType(false),
        m_messageText()
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper ctor()");
        FUNCTION_EXIT;
    }

    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(INVALID_KEY),
        m_operator(INVALID_KEY),
        m_shortName(),
		m_bType(false),
        m_messageText()
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
        reloadUsing(row, data);
        FUNCTION_EXIT;    
    }

    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper(const RadioPredefinedTextMessageHelper& theRadioPredefinedTextMessageHelper)
    :   m_isNew(theRadioPredefinedTextMessageHelper.m_isNew),
        m_isValidData(false),
		m_bType(theRadioPredefinedTextMessageHelper.m_bType),
        m_key(theRadioPredefinedTextMessageHelper.m_key),
        m_operator(theRadioPredefinedTextMessageHelper.m_operator),
        m_shortName(theRadioPredefinedTextMessageHelper.m_shortName),
        m_messageText(theRadioPredefinedTextMessageHelper.m_messageText)
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper copy ctor");
        FUNCTION_EXIT;
    }

    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(key),
		m_bType(false),
        m_operator(INVALID_KEY),
        m_shortName(),
        m_messageText()
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper(const unsigned long operatorid,const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(key),
		m_bType(false),
        m_operator(operatorid),
        m_shortName(),
        m_messageText()
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper ctor(const unsigned long operatorid,const unsigned long key)");
        FUNCTION_EXIT;
    }
    
    RadioPredefinedTextMessageHelper::RadioPredefinedTextMessageHelper(const unsigned long operatorid, const unsigned long messageKey, const std::string messageName, const std::string messageText)
    :   m_isNew(false),
        m_isValidData(false),
        m_key(messageKey),
		m_bType(false),
        m_operator(operatorid),
        m_shortName(messageName),
        m_messageText(messageText)
    {
        FUNCTION_ENTRY("RadioPredefinedTextMessageHelper ctor(const unsigned long operatorid, const unsigned long messageKey, const std::string messageName, const std::string messageText)");
        //check whether it is a new record
        if (!validateKeyExistence())
            m_isNew = true;   
        
        FUNCTION_EXIT;
    }

    RadioPredefinedTextMessageHelper::~RadioPredefinedTextMessageHelper()
    {
        FUNCTION_ENTRY("~RadioPredefinedTextMessageHelper");
        FUNCTION_EXIT;
    }

    //
    // RadioPredefinedTextMessageHelper methods
    //

    unsigned long RadioPredefinedTextMessageHelper::getOperator()
    {
        FUNCTION_ENTRY("getOperator");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_operator;
    }

    std::string RadioPredefinedTextMessageHelper::getShortName()
    {
        FUNCTION_ENTRY("getShortName");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_shortName;
    }

    std::string RadioPredefinedTextMessageHelper::getMessageText()
    {
        FUNCTION_ENTRY("getMessageText");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_messageText;
    }

    void RadioPredefinedTextMessageHelper::setOperator(unsigned long operatorId)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_operator = operatorId;
    }
    
    void RadioPredefinedTextMessageHelper::setShortName(const std::string& shortName)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_shortName = shortName;
    }

	//TD14312
	bool RadioPredefinedTextMessageHelper::getType()
	{
        FUNCTION_ENTRY("getType");
		
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_bType;	
	}

    void RadioPredefinedTextMessageHelper::setMessageText(const std::string& messageText)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_messageText = messageText;
    }

    void RadioPredefinedTextMessageHelper::writePredefinedMessageData()
    {
        FUNCTION_ENTRY("writeTextMessageData");

        // update the database...
        if (m_isNew)
        {
            addNewPredefinedMessage();
        }
        else
        {
            modifyExistingPredefinedMessage();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    void RadioPredefinedTextMessageHelper::deletePredefinedMessage()
    {
        FUNCTION_ENTRY("deletePredefinedMessage");

        TA_ASSERT(! m_isNew, "This Radio Predefined TextMessageInbox does not yet exist in the database and therefore cannot be deleted."); 

       /* std::ostringstream sql;
        sql << "delete " << RA_PREDEFINEDTEXTMESSAGE_TBL << " where " 
            << PKEY_COL << " = " << m_key << " and "
            << OPERATOR_COL << " = " << m_operator;*/

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_DELETE_69005, m_key, m_operator);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_DELETE_69005, m_key, m_operator);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    unsigned long RadioPredefinedTextMessageHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_key;
    }

    void RadioPredefinedTextMessageHelper::invalidate()
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

    void RadioPredefinedTextMessageHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio TextMessage has already been written to the database. It doesn't make
        // any sense to reload an Radio TextMessage that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio Predefined TextMessageInbox does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the data of the radio TextMessage based upon the key
		//TD14312 add the type column
      /*  std::ostringstream sql;
        sql << "select " 
                    << PKEY_COL << ", "
                    << OPERATOR_COL << ", "
                    << SHORTNAME_COL << ", "
					<< TYPE_COL << ", "
                    << MESSAGETEXT_COL << " from " 
                    << RA_PREDEFINEDTEXTMESSAGE_TBL << " where " 
                    << PKEY_COL << " = " << m_key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_SELECT_69003, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_SELECT_69003, m_key);
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(OPERATOR_COL);
		columnNames.push_back(SHORTNAME_COL);
		columnNames.push_back(TYPE_COL);
		columnNames.push_back(MESSAGETEXT_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // radio TextMessage record not found
            std::ostringstream reason;
            reason << "No record found for Radio Predefined TextMessageInbox with key = " << m_key << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Key Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio TextMessageInbox records."); 
                
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void RadioPredefinedTextMessageHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data.getUnsignedLongData(row, PKEY_COL);        
        m_operator = data.getUnsignedLongData(row, OPERATOR_COL);
        m_shortName = data.getStringData(row, SHORTNAME_COL);
        m_messageText = data.getStringData(row, MESSAGETEXT_COL);
		m_bType = static_cast<bool>(data.getIntegerData(row, TYPE_COL));//Haipeng fixed TD14312

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void RadioPredefinedTextMessageHelper::addNewPredefinedMessage()
    {
        FUNCTION_ENTRY("addNewPredefinedMessage");

        unsigned long nextAvailableKey = getNextSequenceNumber();

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        
        // can proceed with adding the new record
		//add new predefined test messge using TYPE = 0 as the default value for TD14312,
        {       
           /* std::ostringstream sql;
            sql << "insert into " 
                        << RA_PREDEFINEDTEXTMESSAGE_TBL 
                        << " (" 
                            << PKEY_COL << ", " 
							<< TYPE_COL << ", " 
                            << OPERATOR_COL << ", "
                            << SHORTNAME_COL << ", "
                            << MESSAGETEXT_COL << ") "
                << "values ("
                        << nextAvailableKey << ", " 
						<< "0, "
                        << m_operator << ", '"
                        << databaseConnection->escapeInsertString(m_shortName) << "', '"
                        << databaseConnection->escapeInsertString(m_messageText) << "')";*/
//			std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_INSERT_69006, nextAvailableKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_INSERT_69006, nextAvailableKey,
				m_operator, databaseConnection->escapeInsertString(m_shortName), databaseConnection->escapeInsertString(m_messageText));

            databaseConnection->executeModification( strSql );

            // update key info for this new record
            m_key = nextAvailableKey;
        }

        FUNCTION_EXIT;    
    }

    void RadioPredefinedTextMessageHelper::modifyExistingPredefinedMessage()
    {
        FUNCTION_ENTRY("modifyExistingPredefinedMessage");

        validateKeyExistence();
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        /*std::ostringstream sql;
        sql << "update " 
                    << RA_PREDEFINEDTEXTMESSAGE_TBL << std::endl
            << "set "
                    << OPERATOR_COL  << " = " << m_operator << ", " << std::endl
                    << SHORTNAME_COL  << " = '" << m_shortName << "', "  << std::endl
                    << MESSAGETEXT_COL  << " = '" << m_messageText << "'" << std::endl
            << "where " 
                    << PKEY_COL << " = " << m_key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_UPDATE_69007, m_operator,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_UPDATE_69007, m_operator,
			m_shortName, m_messageText, m_key);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    bool RadioPredefinedTextMessageHelper::validateKeyExistence()
    {
         // check if radio TextMessage id is in use
      /*  std::ostringstream sql;  
        sql << "select " 
                    << PKEY_COL << std::endl
            <<"from " 
                    << RA_PREDEFINEDTEXTMESSAGE_TBL << std::endl
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
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_SELECT_69004, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_SELECT_69004, m_key);
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (1 == data->getNumRows())
            // cannot find our key in the db
            return true;
        else 
            return false;
    }

    unsigned long RadioPredefinedTextMessageHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
        /*const std::string sql("select RAPTEX_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_PREDEFINED_TEXTMESSAGE_SELECT_69008 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_PREDEFINED_TEXTMESSAGE_SELECT_69008 );
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");

        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }

	void RadioPredefinedTextMessageHelper::setType(bool bType)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_bType = bType;
	}
}
