/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * DisplayRequestAccessFactory is a singleton that is used to generate and retrieve
  * IDisplayRequest objects.
  */

#pragma warning ( disable : 4786 )

#include <string>
#include <sstream>
#include <sys/timeb.h>
#include <time.h>

#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestTable.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_Core
{

    TA_Base_Core::ReEntrantThreadLockable DisplayRequestAccessFactory::m_singletonLock;
    DisplayRequestAccessFactory* DisplayRequestAccessFactory::m_theClassInstance = NULL;

    DisplayRequestAccessFactory& DisplayRequestAccessFactory::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object

            m_theClassInstance = new DisplayRequestAccessFactory();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    std::vector<IDisplayRequest*> DisplayRequestAccessFactory::getDisplayRequests()
    {
        FUNCTION_ENTRY( "getDisplayRequests" );
        FUNCTION_EXIT;
        return getDisplayRequestsWhere();
    }


    std::vector<IDisplayRequest*> DisplayRequestAccessFactory::getDisplayRequestsGreaterThan(time_t time)
    {
        FUNCTION_ENTRY( "getDisplayRequestsGreaterThan" );

        std::stringstream whereSQL;
        // TD 16724
        whereSQL << "where " << TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME
                 << " > " << "TO_DATE("<< timetToString(time) <<", 'YYYYMMDDHH24MISS')";
        // TD 16724
        FUNCTION_EXIT;
        return getDisplayRequestsWhere( whereSQL.str() );
    }


    void DisplayRequestAccessFactory::bulkLoadDisplayRequest( const std::vector< IDisplayRequest* >& requests )
    {
        FUNCTION_ENTRY( "bulkLoadDisplayRequest" );

        typedef std::map< std::string, IDisplayRequest* > DisplayRequestMap;
        DisplayRequestMap lookup;

        for ( std::vector<IDisplayRequest*>::const_iterator iter = requests.begin(); iter != requests.end(); ++iter )
        {
            lookup.insert( DisplayRequestMap::value_type( ( *iter )->getKey(), *iter ) );
        }

        std::stringstream sql;
        sql << "SELECT " << DisplayRequestHelper::getColumnNamesSql();
        sql << "FROM " << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME << " ";
        sql << "ORDER BY " << TA_TISAgentDAI::TIMESTAMP_COLUMN << " DESC ";

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Read);

        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), DisplayRequestHelper::getColumnNames() );

        if ( 0 == data->getNumRows() )
        {
            delete data;
            data = NULL;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "There are no Display Requests in the database table %s",
                         TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME.c_str() );

            FUNCTION_EXIT;
            return;
        }

        for ( bool hasMore = true; hasMore; hasMore = databaseConnection->moreData( data ) )
        {
            boost::shared_ptr< TA_Base_Core::IData > smartData( data );
            data = NULL;

            for ( unsigned long i = 0; i < smartData->getNumRows(); ++i )
            {
                try
                {
                    DisplayRequestMap::iterator findIter = lookup.find( smartData->getStringData( i, TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN ) );

                    if ( lookup.end() != findIter )
                    {
                        findIter->second->populateData( smartData, i );
                    }
                }
                catch(...)
                {
                    // ignore any exception
                }
            }
        }

        FUNCTION_EXIT;
    }


    std::vector<IDisplayRequest*> DisplayRequestAccessFactory::getDisplayRequestsWhere(std::string whereSQL )
    {
        FUNCTION_ENTRY( "getDisplayRequestsWhere" );

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Read);

        // Get all the DisplayRequest primary keys that exist in the table.

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME;
        selectSql << " " << whereSQL;
        selectSql << " order by " << TA_TISAgentDAI::TIMESTAMP_COLUMN << " desc";

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( selectSql.str(), columnNames );

        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no Display Requests in the database table ";
            error << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to IDisplayRequest.
        std::vector< IDisplayRequest* > displayRequests;

        try
        {
            bool moreData = true;
            while (moreData)
            {
                for ( unsigned long i = 0; i < data->getNumRows(); ++i )
                {
                    displayRequests.push_back( new DisplayRequestData( data->getStringData( i, TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN ) ) );
                }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        FUNCTION_EXIT;
        return displayRequests;
    }


    IDisplayRequest* DisplayRequestAccessFactory::getDisplayRequest( const std::string& timestamp )
    {
        FUNCTION_ENTRY( "getDisplayRequest" );

        std::stringstream whereSQL;
        whereSQL << "where " << TA_TISAgentDAI::TIMESTAMP_COLUMN
            << " = '" << timestamp << "'";

        std::vector< IDisplayRequest* > displayRequests; // There should be no more than one

        try
        {
            displayRequests = getDisplayRequestsWhere(whereSQL.str());
        }
        catch ( const TA_Base_Core::DataException& )
        {
            std::stringstream error;
            error << "No Display Requests exist in the database with the specified timestamp";
            error << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        FUNCTION_EXIT;
        return displayRequests[0];
    }


    IDisplayRequest* DisplayRequestAccessFactory::createNewDisplayRequest()
    {
        FUNCTION_ENTRY( "createNewDisplayRequest" );
        FUNCTION_EXIT;
        return new TA_IRS_Core::DisplayRequestData();
    }


    void DisplayRequestAccessFactory::addPidsToExistingTrainDisplayRequest( std::string key,
                                                                            const std::string& pids)
    {
        FUNCTION_ENTRY( "addPidsToExistingTrainDisplayRequest" );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Adding Train PIDs %s to display request %s",
            pids.c_str(), key.c_str() );

        std::ostringstream sql;
        sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        sql << "update " << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME
            << " set " << TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN
            << " = " << TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN << "||'' "<< pids << "''"
            << " where " << TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN << " = ''" << key <<"''";
        sql << "'); ";
        sql << "END;";

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Write);

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    std::string DisplayRequestAccessFactory::getLocalDatabaseName()
    {
        FUNCTION_ENTRY( "getLocalDatabaseName" );

        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                TA_THROW(TA_Base_Core::DatabaseException("db-connection-file not set"));
            }
            try
            {
                TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Write);
                m_localDatabase = connections[0].first;
            }
            catch(...) // most likely a file not found error
            {
                TA_THROW( TA_Base_Core::DatabaseException("Unable to find database connection infomation"));
            }

        }

        FUNCTION_EXIT;
        return m_localDatabase;
    }


    std::string DisplayRequestAccessFactory::timetToString(const time_t time)
    {
        FUNCTION_ENTRY( "timetToString" );

        struct tm newtime;

#if defined ( SOLARIS )
        newtime = *localtime_r ( &time, &newtime );
#else
        newtime = *localtime ( &time);
#endif
        char timeString[100] = {0};
        sprintf ( timeString, "%04d%02d%02d%02d%02d%02d",
                  newtime.tm_year+1900,
                  newtime.tm_mon+1,
                  newtime.tm_mday,
                  newtime.tm_hour,
                  newtime.tm_min,
                  newtime.tm_sec);

        FUNCTION_EXIT;
        return timeString;
    }


} //end namespace TA_IRS_Core


