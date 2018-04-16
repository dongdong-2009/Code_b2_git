/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the Class RadioPredefinedTextMessageFactory
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.h"
#include "core/data_access_interface/radio/src/RadioPredefinedTextMessage.h"
#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageHelper.h"
#include "core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h"
#include "core/data_access_interface/radio/src/ConfigRadioPredefinedTextMessage.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

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
	static const std::string TYPE_COL            = "TYPE";//added by Haipeng for TD14312
}


using namespace TA_Base_Core;

namespace TA_IRS_Core
{


    RadioPredefinedTextMessageFactory* RadioPredefinedTextMessageFactory::m_instance = NULL;


    RadioPredefinedTextMessageFactory::RadioPredefinedTextMessageFactory()
    {
    }

    RadioPredefinedTextMessageFactory::~RadioPredefinedTextMessageFactory()
    {
    }

    RadioPredefinedTextMessageFactory& RadioPredefinedTextMessageFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if(NULL == m_instance)
        {
            m_instance = new RadioPredefinedTextMessageFactory();
        }
        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioPredefinedTextMessageFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    IRadioPredefinedTextMessage* RadioPredefinedTextMessageFactory::getRadioPredefinedTextMessage(const unsigned long key, const bool bStatus, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getRadioPredefinedTextMessage");
    
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
            << " from "  << RA_PREDEFINEDTEXTMESSAGE_TBL
            << " where " << PKEY_COL << " = " << key 
			<< " and " << TYPE_COL << " = " << "'" << bStatus << "'";*///added by Haipeng for TD14312
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_PREDEFINED_TEXTMESSAGE_SELECT_69001, key, bStatus );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_PREDEFINED_TEXTMESSAGE_SELECT_69001, key, bStatus );

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
		    reason << "No record found for Radio Predefined TextMessage with key = " << key;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Invalid Radio Predefined TextMessage primary key." ) );
        }

        // Don't need to do anything with the dataa, only needed to check if it exists
        TA_ASSERT(1 == data->getNumRows(), "Invalid number of records.");        

        // Create the radio textmessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioPredefinedTextMessage* theTextmessage = NULL;
        
        // decide the type of radio textmessage object to return based on the readWrite param
        if (readWrite == true)
        {
			//fixed TD13419
            theTextmessage = new ConfigRadioPredefinedTextMessage(key);
        }
        else
        {
            theTextmessage = new RadioPredefinedTextMessage(key);
        }

        // Return the radio textmessage pointer. The caller is responsible for deleting it.
        FUNCTION_EXIT;
        return theTextmessage;
    }

    std::vector<IRadioPredefinedTextMessage*> RadioPredefinedTextMessageFactory::getAllRadioPredefinedTextMessages(const std::string& sessionId, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getAllRadioPredefinedTextMessages");

        unsigned long operatorId = getOperatorKey(sessionId);

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve all the records from the table 
       /* std::ostringstream sql;
        sql << "select " << PKEY_COL << ", " 
                         << OPERATOR_COL << ", " << SHORTNAME_COL << ", "
                         << MESSAGETEXT_COL << ", " << TYPE_COL
            << " from "  << RA_PREDEFINEDTEXTMESSAGE_TBL
            << " WHERE " << OPERATOR_COL << " = " << operatorId;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_PREDEFINED_TEXTMESSAGE_SELECT_69002, operatorId);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_TEXTMESSAGE_SELECT_69002, operatorId);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
		columnNames.push_back(OPERATOR_COL);
        columnNames.push_back(SHORTNAME_COL);
        columnNames.push_back(MESSAGETEXT_COL);
		columnNames.push_back(TYPE_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioPredefinedTextMessageHelper
        std::vector<IRadioPredefinedTextMessage*> messagePtrVector;

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
						//fixed TD13419
                        messagePtrVector.push_back(new ConfigRadioPredefinedTextMessage(i, *data));
                    }
                    else
                    {
                        messagePtrVector.push_back(new RadioPredefinedTextMessage(i, *data));
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

    void RadioPredefinedTextMessageFactory::changedPredefinedTextMessage(const std::string& sessionId, const unsigned long messageKey, const std::string messageName, const std::string messageText)
    {
        FUNCTION_ENTRY("changedPredefinedTextMessage");

        unsigned long operatorId = getOperatorKey(sessionId);

        try
        {
            RadioPredefinedTextMessageHelper helper(operatorId, messageKey, messageName, messageText);
            helper.writePredefinedMessageData();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to change the predefined text message with id: %d.", messageKey );
            throw;  
        }
        FUNCTION_EXIT;
    }

    void RadioPredefinedTextMessageFactory::deletePredefinedTextMessage(const std::string& sessionId,const unsigned long messageid)
    {
        FUNCTION_ENTRY("deletePredefinedTextMessage");
        try
        {
			unsigned long operatorId = getOperatorKey(sessionId);

            RadioPredefinedTextMessageHelper helper(messageid);
			helper.setOperator(operatorId);
            helper.deletePredefinedMessage();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to delete the predefined text message with id: %d.", messageid );
            throw;  
        }
        FUNCTION_EXIT;
    }

    IRadioPredefinedTextMessage* RadioPredefinedTextMessageFactory::createNew(const std::string& sessionId, const std::string messageName, const std::string messageText)
    {
        unsigned long operatorId = getOperatorKey(sessionId);

        ConfigRadioPredefinedTextMessage* data = new ConfigRadioPredefinedTextMessage();
        data->setOperator(operatorId);
        data->setShortName(messageName);
        data->setMessageText(messageText);
		data->setType(false);
        data->save();

        return data;
    }

    unsigned long RadioPredefinedTextMessageFactory::getOperatorKey(const std::string& sessionId)
    {
        TA_Base_Core::ISession* session = NULL;
        
        unsigned long operatorId = 0;
        
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
        
        return operatorId;
    }

}


