/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInboxFactory.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the Class RadioTextMessageInboxFactory
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxFactory.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/ConfigRadioTextMessageInbox.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

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
	static const std::string RECIPIENTCONSOLE_COL = "RECIPIENTCONSOLE";
}

using namespace TA_Base_Core;

namespace TA_IRS_Core
{


    RadioTextMessageInboxFactory* RadioTextMessageInboxFactory::m_instance = NULL;


    RadioTextMessageInboxFactory::RadioTextMessageInboxFactory()
    {
    }

    RadioTextMessageInboxFactory::~RadioTextMessageInboxFactory()
    {
    }

    RadioTextMessageInboxFactory& RadioTextMessageInboxFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if(NULL == m_instance)
        {
            m_instance = new RadioTextMessageInboxFactory();
        }
        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioTextMessageInboxFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    IRadioTextMessageInbox* RadioTextMessageInboxFactory::getRadioTextMessageInbox(const unsigned long key, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getRadioTextMessageInbox");
    
        TA_ASSERT(key > 0, "Invalid Radio TextMessage Id index.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the name of the radio textmessage
        // this is a check to ensure an radio textmessage with the specified key actually exists.        
       /* std::ostringstream sql;
        sql << "select " << PKEY_COL 
            << " from "  << RA_TEXTMESSAGEINBOX_TBL
            << " where " << PKEY_COL << " = " << key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_TEXTMESSAGE_INBOX_SELECT_71001, key );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_TEXTMESSAGE_INBOX_SELECT_71001, key );

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);	

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        if (0 == data->getNumRows())
        {
            // radio textmessage record with the specified primary key not found
            std::ostringstream reason;
		    reason << "No record found for Radio TextMessage with key = " << key;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Invalid Radio TextMessage primary key." ) );
        }

        // Don't need to do anything with the dataa, only needed to check if it exists
        TA_ASSERT(1 == data->getNumRows(), "Invalid number of records.");        

        // Create the radio textmessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioTextMessageInbox* theTextmessage = NULL;
        
        // decide the type of radio textmessage object to return based on the readWrite param
        if (readWrite == true)
        {
            theTextmessage = new ConfigRadioTextMessageInbox(key);
        }
        else
        {
            theTextmessage = new RadioTextMessageInbox(key);
        }

        // Return the radio textmessage pointer. The caller is responsible for deleting it.
        FUNCTION_EXIT;
        return theTextmessage;
    }

    std::vector<IRadioTextMessageInbox*> RadioTextMessageInboxFactory::getAllRadioTextMessageInboxs(const std::string& sessionId, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getAllRadioTextMessageInboxs");

		//unsigned long operatorId = getOperatorKey(sessionId);

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve all the records from the table 
   //     std::ostringstream sql;
   //     sql << "select " << PKEY_COL << ", " << "to_char(" << TIMESTAMP_COL << ", 'YYYYMMDDHH24MMSS'), "
   //                      << OPERATOR_COL << ", " << ISREAD_COL << ", "
			//			 << MESSAGETEXT_COL << ", "
			//			 << RECIPIENTCONSOLE_COL << ","
   //                      << SOURCEALIAS_COL
   //         << " from "  << RA_TEXTMESSAGEINBOX_TBL
   //         //<< " WHERE " << OPERATOR_COL << " = " << operatorId;
			//<< " WHERE " << RECIPIENTCONSOLE_COL << " = '" << databaseConnection->escapeInsertString(sessionId) << "' "
			//<< " OR " << RECIPIENTCONSOLE_COL << " = 'ALL'";
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_TEXTMESSAGE_INBOX_SELECT_71002, databaseConnection->escapeInsertString(sessionId) );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_TEXTMESSAGE_INBOX_SELECT_71002, databaseConnection->escapeInsertString(sessionId) );
         
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
        columnNames.push_back(TIMESTAMP_COL);
		columnNames.push_back(OPERATOR_COL);
        columnNames.push_back(ISREAD_COL);       
        columnNames.push_back(MESSAGETEXT_COL);   
		columnNames.push_back(RECIPIENTCONSOLE_COL);
		columnNames.push_back(SOURCEALIAS_COL);   
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioTextMessageInboxHelper
        std::vector<IRadioTextMessageInbox*> messagePtrVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);

                TA_ASSERT(key > 0, "Invalid messagetext Id index."); 

                if (key > 0)
                {
                    if (readWrite == true)
                    {
                        messagePtrVector.push_back(new ConfigRadioTextMessageInbox(i, *data));
                    }
                    else
                    {
                        messagePtrVector.push_back(new RadioTextMessageInbox(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return messagePtrVector;
    }
    

	IRadioTextMessageInbox* RadioTextMessageInboxFactory::addNew(
		time_t timestamp, const std::string& sessionId, char isread,
		const std::string& msgtext,const std::string& source, const std::string& recipient)
    {
        ConfigRadioTextMessageInbox* data = new ConfigRadioTextMessageInbox();
		data->setTimeStamp(timestamp);
		data->setMessageText(msgtext);
		data->setSourceAlias(source);
		data->setOperator(getOperatorKey(sessionId));
		data->setRecipientConsole(recipient);
		
        data->save();
        return data;
    }
   
    unsigned long RadioTextMessageInboxFactory::getOperatorKey(const std::string& sessionId)
    {
        unsigned long operatorId = 0;
        /* TODO: for now use 0 for operator ID, until we know how to deliver the message
        TA_Base_Core::ISession* session = NULL;

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
        */

		return operatorId;
    }


	void RadioTextMessageInboxFactory::setRead(const unsigned long key , bool flag)
    {
        ConfigRadioTextMessageInbox data(key);
		
		data.setRead(flag);
        data.save();
	}

}



