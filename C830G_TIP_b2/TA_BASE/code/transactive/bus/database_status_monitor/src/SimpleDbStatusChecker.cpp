/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/database_status_monitor/src/SimpleDbStatusChecker.cpp $
 * @author:  J.Chevalley
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2011/03/29 13:47:32 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "bus/database_status_monitor/src/SimpleDbStatusChecker.h"
#include "bus/database_status_monitor/src/IDatabaseStatusObserver.h"
#include "core/database/src/SimpleDb.h"
#include "core/database/src/DBException.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"

//TD18251, jianghp, to replace the OCI with OCL
#include "core/ocl/src/ocl.h"


using namespace TA_Base_Core;

using namespace ocl;

namespace TA_Base_Bus
{

    SimpleDbStatusChecker::SimpleDbStatusChecker(const std::string& connectionInfo, unsigned long periodInSeconds, IDatabaseStatusObserver* observer)
        :m_dbConnectionInfo(connectionInfo),
         m_checkPeriodInSeconds(periodInSeconds),
         m_dbStatusObserver(observer),
         m_dbStatus(DatabaseStatusCorbaDef::statusUnknown),
		 m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
		 m_reConnect(false),
		 m_connection(0)
    {
        std::vector<std::string> components;
        SimpleDb::ParseConnectStr(connectionInfo, components, ':');

        m_hostName = components[0];
        m_schemaName = components[1];
        m_dbPassword = components[2];
		Init();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"[DEBUG marvin]SimpleDbStatusChecker::Connect() created for hostname %s",m_hostName.c_str());
    }
	SimpleDbStatusChecker::~SimpleDbStatusChecker()
	{
		delete m_pQuery;			
		if (m_connection != 0 && m_connection->isConnected())
		{
			m_connection->close();
			delete m_connection;
			m_connection = NULL;
		}
	}
	
    void SimpleDbStatusChecker::setDatabaseStatus(DatabaseStatusCorbaDef::EState status)
    {

	    //Updates the stored database status.
	    //If there has been a change in status
	    //the observer is notified.
        if (m_dbStatus != status)
        {
            m_dbStatusObserver->notifyStatusChange(m_dbConnectionInfo, status);
            m_dbStatus = status;
        }
    }

    void SimpleDbStatusChecker::run()
    {
        // are/can we connect?
        if (!Connect())
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
				"SimpleDbStatusChecker::run() connect failed for database server %s. Database set to UNAVAILABLE",m_hostName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
			m_reConnect = true;
            //return;
        }
		m_timerUtility.startPeriodicTimeOutClock(this, 300000);
        m_run = true;
        while(m_run)
        {
            Poll();
            
            // thread sleep is in milliseconds
            sleep(m_checkPeriodInSeconds * 1000);
        }

		m_timerUtility.stopPeriodicTimeOutClock(this);
    }


    bool SimpleDbStatusChecker::Connect()
    {
		// not currently connected - try to connect
		try
		{
			TA_ASSERT(NULL != m_connection, "connection pointer is null");
			if (m_connection->isConnected())		
				return true;		
			else		
				m_connection->open();
		}
		catch(ocl::OraException & e)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s. \nOCI_ERROR:[%s]",m_hostName.c_str(), e.message().c_str());
			if(m_connection->isConnected())
				m_connection->close();
			return false;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s. Returning false",m_hostName.c_str());
			if(m_connection->isConnected())
				m_connection->close();
			return false;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"SimpleDbStatusChecker::Connect() Successfull connection to databse server %s",m_hostName.c_str());
		return true;
    }


    void SimpleDbStatusChecker::Poll()
	{
		try
		{
			if (true == m_reConnect)
			{
				if (!Connect())
				{// if can not connect, continuely trying to connect DB in next loop.
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
							"SimpleDbStatusChecker::Poll() connect failed for database server %s. Database set to UNAVAILABLE",m_hostName.c_str());
					setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
					m_reConnect = true;// 
					return;
				}
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"SimpleDbStatusChecker::Poll() Executing dummy select for database server %s.",m_hostName.c_str());
            
			//create a query to test DB server usability.
			TA_ASSERT(m_connection != NULL, "m_connection should not be NULL");
			if (!m_connection->isConnected())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
					"SimpleDbStatusChecker::Poll() Database is not connected %s. Database status UNAVAILABLE",m_hostName.c_str());
				setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
				m_reConnect = true;
				return;
			}

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "query:%p m_connection:%p", m_pQuery, m_connection );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "set connection %s for query", m_hostName.c_str() );
			m_pQuery->setConnection(*m_connection);
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "set command type ocl::ctSQL for query" );
			m_pQuery->setCommandType(ocl::ctSQL);
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "set cached to false for query" );
			m_pQuery->setCached(false);  // noncached mode
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "set fetch rows to 100 for query" );
			m_pQuery->setFetchRows(100);
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "set SQL [select * from V$session] for query" );
			m_pQuery->setSQL("select * from V$version");
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "try to open query" );
			m_pQuery->open();   // query result rows
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "query opened" );
			m_connection = m_pQuery->connection();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "calling isRowsReturn() on query" );
			if (m_pQuery->isRowsReturn())
			{
				m_pQuery->close();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"SimpleDbStatusChecker::Poll() Database poll successfull for database server %s. Database status AVAILABLE",m_hostName.c_str());
				setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
				m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.
				
                return;
			}else
			{
				m_pQuery->close();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
					"SimpleDbStatusChecker::Poll() Database returned empty result for database server  %s. Database status UNAVAILABLE",m_hostName.c_str());
				setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
				m_reConnect = true;
			}
		}
		catch(ocl::OraException &e )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_hostName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; \n will re-connect database", e.errorCode(), e.message().c_str());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
			{
				m_reConnect =  true;
				if ( m_pQuery && m_pQuery->isActive() )
					m_pQuery->close();
				m_connection = m_pQuery->connection();
			}	
		    setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }
		catch (CRException& )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_hostName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
			m_reConnect =  true;
			if ( m_pQuery && m_pQuery->isActive() )
				m_pQuery->close();
			m_connection = m_pQuery->connection();
		}
        catch(...)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Poll() Caught exception while trying to connect to databse server %s.",m_hostName.c_str());
            return ;
        }
    }

	void SimpleDbStatusChecker::Init()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
			"SimpleDbStatusChecker::Connect() create a new connection to database server %s...",m_hostName.c_str());
		OraEnvironment::setInitMode(OraEnvironment::imObject);
		OraEnvironment::setInitMode(OraEnvironment::imThreaded);
		OraEnvironment::setThreaded(true);
		m_connection = new ocl::OraConnection();
		m_connection->setConnectMode(cmNormal);
		m_connection->setUsername(m_schemaName); 
		m_connection->setPassword(m_dbPassword); 
		m_connection->setServer(m_hostName); 

		// initialize the query result set
		m_pQuery = new OraQuery;
	}

    void SimpleDbStatusChecker::terminate()
    {
        m_run = false;
    }

	void SimpleDbStatusChecker::timerExpired(long timerId, void* userData)
	{
		m_dbStatusObserver->notifyStatusChange(m_dbConnectionInfo, m_dbStatus);
	}

	void SimpleDbStatusChecker::setDatabaseOffLine()
	{
		setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
	}


}//namespace


