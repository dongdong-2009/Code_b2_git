/**
  * DataBaseConnection.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/DataBaseConnection.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class loads creates a database connection for use throughout
  * the program.  This class is a singleton.
  * NOTE: This class uses Oracle OCCI which is new to Oracle 9i.  To
  * run and compile, it is necessary to have the Oracle 9i client.
  *
  */

//                       *** IMPORTANT NOTE ***
// If linking with OCCI, you will need to add oraocci9.lib and msvcprt.lib
// to the list of libraries.  msvcprt.lib is required to prevent problems
// with the OmniORB libraries and the OCCI libraries using different
// std::string's which causes a problem when deallocating the string.

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/historyviewer.h"
#include "app/history/history_viewer/src/DataBaseConnection.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/database/src/SimpleDb.h"
#include "core/database/src/DBException.h"

using namespace TA_Core;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


namespace TA_App
{

#ifdef USE_OCCI

    //////////////////////////////////////////////////////////////////////
    // Destruction
    //////////////////////////////////////////////////////////////////////

    DataBaseConnection::~DataBaseConnection()
    {
        if( m_Connected == true && env )
        {
	        if( conn )
	        {
		        env->terminateConnection( conn );
	        }
	        oracle::occi::Environment::terminateEnvironment( env );
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////

    // Create the connection
    void DataBaseConnection::CreateConnection()
    {
        if( m_Connected == true )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Database connection already open.");
	        return;
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Obtaining database connection");

        std::string connectionString = RunParams::GetInstance().Get(RPARAM_DBCONNECTION);

        // Parse the Connection String to separate the database connection information.
        std::vector<std::string> dbConnData;

        SimpleDb::ParseConnectStr(connectionString, dbConnData);
  
        // 3 is used because dbConnData should consist of servicename:username:password
        if (dbConnData.size() < 3)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal,"Invalid Db Connection String");
	        throw CouldNotConnectException();
        }

        // Open the database connection using the parsed information.
        try
        {
	        // env = oracle::occi::Environment::createEnvironment( oracle::occi::Environment::DEFAULT );
	        env = oracle::occi::Environment::createEnvironment( oracle::occi::Environment::OBJECT );
	        conn = env->createConnection( dbConnData[1], dbConnData[2], dbConnData[0] );
	        m_Connected = true;
        }
        catch ( oracle::occi::SQLException &sqlException )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not obtain the database connection.");

	        // Log the Oracle Error Message (includes error number)
	        std::string oracleErrorMsg = sqlException.getMessage();
	        LOGMORE(SourceInfo, oracleErrorMsg.c_str());
	        throw;  // Rethrow
        }
    }

    // Get the connection
    oracle::occi::Connection* DataBaseConnection::GetConnection()
    {
        // If the connection has not yet been created, do it.
        // NOTE: This can throw an exception which needs to be handled by the caller
        if( m_Connected == false )
        {
	        CreateConnection();
        }

        // If we still don't have a connection, throw an exception
        if( !conn )
        {
	        throw CouldNotConnectException();
        }

        return conn;
    }

    #else // USE_OCCI

    //////////////////////////////////////////////////////////////////////
    // Destruction
    //////////////////////////////////////////////////////////////////////

    SimpleDBConnection::~SimpleDBConnection()
    {
        // Delete the connection
        ClearConnection();
    }

    //////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////

    // Delete the connection
    // IMPORTANT NOTE: This must be called when closing the dialog.  If not,
    // the OCIAccess singleton may be called before the SimpleDBConnection
    // singleton is destroyed.  If this occurs, when the SimpleDBConnection
    // singleton is destroyed, SimpleDb will attempt to close a connection
    // on an object that has already been destroyed - This is bad...
    void SimpleDBConnection::ClearConnection()
    {
        if( m_SimpleDB )
        {
	        delete m_SimpleDB;  // Closed on destruction
	        m_SimpleDB = NULL;
        }
    }

    // Create the connection
    void SimpleDBConnection::CreateConnection()
    {
        if( m_Connected == true )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Database connection already open.");
	        return;
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Obtaining database connection");

        std::string connectionString = RunParams::getInstance().get(RPARAM_DBCONNECTION);

        // Parse the Connection String to separate the database connection information.
        std::vector<std::string> dbConnData;

        SimpleDb::ParseConnectStr(connectionString, dbConnData);
  
        // 3 is used because dbConnData should consist of servicename:username:password
        if (dbConnData.size() < 3)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal,"Invalid Db Connection String");
	        throw CouldNotConnectException();
        }

        // Delete the connection (in case CreateConnection is called twice)
        ClearConnection();

		try
		{
			// Create the SimpleDb object using the parsed information
			m_SimpleDB = new SimpleDb(dbConnData[0].c_str(), dbConnData[1].c_str(), dbConnData[2].c_str());
		}
		catch( const DBException& e )
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );

	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "Could not open database connection" );

	        throw CouldNotConnectException();
        }
        m_Connected = true;
    }

    // Get the connection
    SimpleDb* SimpleDBConnection::GetConnection()
    {
        // If the connection has not yet been created, do it.
        // NOTE: This can throw an exception which needs to be handled by the caller
        if( m_Connected == false )
        {
	        CreateConnection();
        }

        // If we still don't have a connection, throw an exception
        if( !m_SimpleDB )
        {
	        throw CouldNotConnectException();
        }

        return m_SimpleDB;
    }

    #endif // USE_OCCI
}

