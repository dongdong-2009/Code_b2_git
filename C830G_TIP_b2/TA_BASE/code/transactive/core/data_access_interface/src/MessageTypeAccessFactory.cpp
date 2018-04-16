/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/MessageTypeAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * MessageTypeAccessFactory is a singleton that is used to retrieve MessageTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/data_access_interface/src/ConfigMessageType.h"
#include "core/data_access_interface/src/MessageTypeData.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    MessageTypeAccessFactory* MessageTypeAccessFactory::m_instance = 0;

    MessageTypeAccessFactory& MessageTypeAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new MessageTypeAccessFactory();
        }
        return *m_instance;
    }

    void MessageTypeAccessFactory::getMessageTypeBySql(const std::string& sql, std::vector<IMessageTypeData*>& messageTypes, const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("MEMTYP_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PRIORITY");
        columnNames.push_back("PRINTMESSAGE");
        columnNames.push_back("LOGMESSATE");
        columnNames.push_back("PERSISTMESSAGE");
		columnNames.push_back("VIEWLEVEL");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        try
        {
            do
            {
                for(unsigned long i = 0; i < data->getNumRows(); i ++)
                {
                    // decide which message type object to return
                    if (readWrite) // need a config message type
                    {
                        messageTypes.push_back( new ConfigMessageType( i, *data));
                    }
                    else // need a standard message type
                    {
						messageTypes.push_back( new MessageTypeData( i, *data));
                    }
                }

                delete data;
                data = NULL;
            }
            while ( databaseConnection->moreData(data) );

        }
        catch(...)
        {
            delete data;
            data = NULL;
			throw;
        }
    }


    IMessageTypeData* MessageTypeAccessFactory::getMessageTypeByKey(const std::string& key,const bool readWrite)
    {
		FUNCTION_ENTRY("getMessageTypeByKey");

		// create the SQL string to retrieve the key of the message type
        std::ostringstream sql;
		sql << "SELECT memtyp_id, type_name, NVL(type_description,'NULL') as type_description, priority, print_message, log_message, persist_message,"
            << "view_level, TO_CHAR(date_modified, 'YYYYMMDDHH24MISS'), TO_CHAR(date_created,'YYYYMMDDHH24MISS') "
            << "FROM me_message_type where memtyp_id = '" << key << "'";
		
		std::vector<IMessageTypeData*> messageTypes;
		getMessageTypeBySql(sql.str(), messageTypes, readWrite);

        if ( 0 == messageTypes.size())
        {
            std::ostringstream message;
			message << "No data found for MessageType with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == messageTypes.size(), "MessageType key unique constraint violated");

        // Return the message type pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return messageTypes[0];
    }

    IMessageTypeData* MessageTypeAccessFactory::getMessageType(const std::string& name,const bool readWrite)
    {
		FUNCTION_ENTRY("getMessageType");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve the key of the message type
        std::ostringstream sql;
		sql << "SELECT memtyp_id, type_name, NVL(type_description,'NULL') as type_description, priority, print_message, log_message, persist_message,"
            << "view_level, TO_CHAR(date_modified, 'YYYYMMDDHH24MISS'), TO_CHAR(date_created,'YYYYMMDDHH24MISS') "
            << "FROM me_message_type where type_name = '" << databaseConnection->escapeQueryString(name) << "'";

		std::vector<IMessageTypeData*> messageTypes;
		getMessageTypeBySql(sql.str(), messageTypes, readWrite);

        if ( 0 == messageTypes.size())
        {
            std::ostringstream message;
			message << "No data found for MessageType with name " << name;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == messageTypes.size(), "MessageType key unique constraint violated");

        // Return the message type pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return messageTypes[0];
    }


    std::vector<IMessageTypeData*> MessageTypeAccessFactory::getAllMessageTypes(
        const bool readWrite, const std::vector<std::string>& eventChannels)
    {
		FUNCTION_ENTRY("getAllMessageTypes");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Create the sql to select all message types from the database
        std::ostringstream sql;
		sql << "SELECT memtyp_id, type_name, NVL(type_description,'NULL') as type_description, priority, print_message, log_message, persist_message,"
            << "view_level, TO_CHAR(date_modified, 'YYYYMMDDHH24MISS'), TO_CHAR(date_created,'YYYYMMDDHH24MISS') "
            << "FROM me_message_type";

        // If at least one event channel has been specified, filter the
        // results to those channels specified.
        if(eventChannels.size() > 0)
        {
            // Get the channels as a comma-separated list
            std::string channelList = "'";
            for(std::vector<std::string>::const_iterator it = eventChannels.begin();
                it != eventChannels.end();
                it ++)
            {
                if( it != eventChannels.begin() )
                {
                    channelList += "', '";
                }

                channelList += databaseConnection->escapeQueryString(*it);
            }
            channelList += "'";

            // Add the filtering sql
            sql << " where memcon_id in "
                <<     "(select memcon_id from me_message_context "
                <<     " where meecha_id in "
                <<         "(select meecha_id from me_event_channel "
                <<         " where channel_name in (" << channelList << ")"
                <<         ")"
                <<     ")";
        }

		// TD15195 hide unwanted message type
		sql << " and isvisible = 1";

		std::vector<IMessageTypeData*> messageTypes;
		getMessageTypeBySql(sql.str(), messageTypes, readWrite);

        // Return the message type pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return messageTypes;
    }


    std::vector<IMessageTypeData*> MessageTypeAccessFactory::getAllMessageTypesAndAttributes(
        const std::vector<std::string>& eventChannels)
    {
		FUNCTION_ENTRY("getAllMessageTypesAndAttributes");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Create the sql to select all message types from the database
        std::ostringstream sql;
		sql << "SELECT memtyp_id, type_name, NVL(type_description,'NULL') as type_description, priority, print_message, log_message, persist_message,"
            << "view_level, TO_CHAR(date_modified, 'YYYYMMDDHH24MISS'), TO_CHAR(date_created,'YYYYMMDDHH24MISS') "
            << "FROM me_message_type";

        // TD15195 hide unwanted message type
        sql << " where isvisible = 1";
        
        // If at least one event channel has been specified, filter the
        // results to those channels specified.
        if(eventChannels.size() > 0)
        {
            // Get the channels as a comma-separated list
            std::string channelList = "'";
            for(std::vector<std::string>::const_iterator it = eventChannels.begin();
                it != eventChannels.end();
                it ++)
            {
                if( it != eventChannels.begin() )
                {
                    channelList += "', '";
                }

                channelList += databaseConnection->escapeQueryString(*it);
            }
            channelList += "'";

            // Add the filtering sql
            sql << " and memcon_id in "
                <<     "(select memcon_id from me_message_context "
                <<     " where meecha_id = "
                <<         "(select meecha_id from me_event_channel "
                <<         " where channel_name in (" << channelList << ")"
                <<         ")"
                <<     ")";
        }

		std::vector<IMessageTypeData*> messageTypes;
		getMessageTypeBySql(sql.str(), messageTypes, false);

        // Return the message type pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return messageTypes;
    }

} //closes TA_Base_Core


