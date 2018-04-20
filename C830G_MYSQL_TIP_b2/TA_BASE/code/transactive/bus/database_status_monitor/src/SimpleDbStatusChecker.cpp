/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/database_status_monitor/src/SimpleDbStatusChecker.cpp $
 * @author:  J.Chevalley
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2012/11/28 11:21:25 $
 * Last modified by:  $Author: weikun.lin $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "driver/mysql_public_iface.h"  // oyzl++ for mysql connection

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

#define  defOracleDBName          "Oracle"
#define  defMysqlDBName           "Mysql"

#define  defOracleDBType           1
#define  defMysqlDBType            2

    SimpleDbStatusChecker::SimpleDbStatusChecker(const std::string& connectionInfo, unsigned long periodInSeconds, IDatabaseStatusObserver* observer)
        :m_dbConnectionInfo(connectionInfo),
         m_checkPeriodInSeconds(periodInSeconds),
         m_dbStatusObserver(observer),
         m_dbStatus(DatabaseStatusCorbaDef::statusUnknown),
		 m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
		 m_reConnect(false),
		 m_pOraConnection(0),
		 m_pMysqlConntion(0),  // oyzl++ for mysql connection
		 m_pMysqlDriver(0),
		 m_pMysqlQuery(0)
    {
        std::vector<std::string> components;
        SimpleDb::ParseConnectStr(connectionInfo, components, ':');
		TA_ASSERT( 5 == components.size(), "Invalid connection string." );

		ParseDBType( components[0] );    // oyzl++ for supporting multiple DB server (Oracle & Mysql)
        m_schemaName = components[1];
		m_strUserName = components[2];
		m_dbPassword  = components[3];
        m_hostName    = components[4];       
		Init();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
			"[DEBUG marvin]SimpleDbStatusChecker::Connect() created for hostname %s, schemaName %s, userName: %s, password %s",
			m_hostName.c_str(), m_schemaName.c_str(), m_strUserName.c_str(), m_dbPassword.c_str() );
    }
	SimpleDbStatusChecker::~SimpleDbStatusChecker()
	{
		delete m_pOraQuery;	  m_pOraQuery = NULL;	
		delete m_pMysqlQuery; m_pMysqlQuery = NULL;
		if (m_pOraConnection != 0 && m_pOraConnection->isConnected())
		{
			m_pOraConnection->close();
			delete m_pOraConnection;
			m_pOraConnection = NULL;
		}
		if (m_pMysqlConntion != 0 && !m_pMysqlConntion->isClosed())
    {
			m_pMysqlConntion->close();
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
		}
	}
	
    void SimpleDbStatusChecker::setDatabaseStatus(DatabaseStatusCorbaDef::EState status)
    {

	    //Updates the stored database status.
	    //If there has been a change in status
	    //the observer is notified.
        if (m_dbStatusObserver && m_dbStatus != status)
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
				"SimpleDbStatusChecker::run() connect failed for database server %s. Database set to UNAVAILABLE",m_schemaName.c_str());
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
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"SimpleDbStatusChecker::Connect() create a new connection to database server %s...",m_schemaName.c_str());
			switch ( m_nDBType )
		{
			case defOracleDBType:
				OraConnection();
				break;
			case defMysqlDBType:
				MysqlConnect();
				break;
			default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s",m_dbConnectionInfo.c_str());
				break;
		}
			
        }
		catch(ocl::OraException & e)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. OCI_ERROR:[%s] ",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.message().c_str());
			if(m_pOraConnection->isConnected())
				m_pOraConnection->close();
            return false;
        }
		catch(sql::SQLException & e)
	{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. Mysql_ERROR:[%s] ",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.what());
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
			return false;
		}
        catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s. Returning false",m_schemaName.c_str());
			if(m_pOraConnection->isConnected())
				m_pOraConnection->close();
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
            return false;
        }
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"SimpleDbStatusChecker::Connect() Successfull connection to databse server %s",m_schemaName.c_str());
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
							"SimpleDbStatusChecker::Poll() connect failed for database server %s. Database set to UNAVAILABLE",m_schemaName.c_str());
				setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
					m_reConnect = true;// 
				return;
			}
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"SimpleDbStatusChecker::Poll() Executing dummy select for database %s.",m_schemaName.c_str());
			//create a query to test DB server usability.
			switch ( m_nDBType )
			{
			case defOracleDBType:
				DoOraQuery();
				break;
			case defMysqlDBType:
				DoMysqlQuery();
				break;
			default:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s",m_dbConnectionInfo.c_str());
				break;
			}
			
		}
		catch(ocl::OraException &e )
			{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_schemaName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.errorCode(), e.message().c_str());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
			{
				m_reConnect =  true;
				if ( m_pOraQuery && m_pOraQuery->isActive() )
				m_pOraQuery->close();
				m_pOraConnection = m_pOraQuery->connection();
			}	
		    setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }
		catch(sql::SQLException &e )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_schemaName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.getErrorCode(), e.what());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
			{
			m_reConnect =  true;
				if ( m_pMysqlQuery && !m_pMysqlQuery->isClosed() )
				m_pMysqlQuery->close();
		}
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
		}
		catch (CRException& )
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
			m_reConnect =  true;
			if ( m_pOraQuery && m_pOraQuery->isActive() )
			m_pOraQuery->close();
			m_pOraConnection = m_pOraQuery->connection();
        }
        catch(...)
    {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"SimpleDbStatusChecker::Poll() Caught exception while trying to connect to databse server %s.",m_schemaName.c_str());
            return ;
    }
	}

	void SimpleDbStatusChecker::OraConnection()
	{
		TA_ASSERT(NULL != m_pOraConnection, "connection pointer is null");
		if (m_pOraConnection->isConnected())		
			return;		
		else		
			m_pOraConnection->open();				
	}

	void SimpleDbStatusChecker::MysqlConnect()
		{			
		if ( NULL != m_pMysqlConntion )
		{			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"SimpleDbStatusChecker::Connect() free Oracle old connection to Mysql database server %s...",m_hostName.c_str());
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
	}
		TA_ASSERT(NULL != m_pMysqlDriver, "the mysql driver is null");
		sql::ConnectOptionsMap connection_properties;
		connection_properties["hostName"] = m_hostName;
		connection_properties["userName"] = m_strUserName;
		connection_properties["password"] = m_dbPassword;
		connection_properties["metadataUseInfoSchema"] = true;
		
		m_pMysqlConntion = m_pMysqlDriver->connect(connection_properties);

		m_pMysqlConntion->setSchema(m_schemaName);
	}

	void SimpleDbStatusChecker::DoOraQuery()
	{
		TA_ASSERT(m_pOraConnection != NULL, "m_connection should not be NULL");
		if (!m_pOraConnection->isConnected())
		{			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"SimpleDbStatusChecker::Poll() Database is not connected %s. Database status UNAVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
			m_reConnect = true;
			return;
		}

		m_pOraQuery->setConnection(*m_pOraConnection);
		m_pOraQuery->setCommandType(ocl::ctSQL);
		m_pOraQuery->setCached(false);  // noncached mode
		m_pOraQuery->setFetchRows(100);
		m_pOraQuery->setSQL("select * from V$version");
		m_pOraQuery->open();   // query result rows
		m_pOraConnection = m_pOraQuery->connection();
		if (m_pOraQuery->isRowsReturn())
	{		
			m_pOraQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"SimpleDbStatusChecker::Poll() Database poll successfull for database server %s. Database status AVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
			m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.

			return;
		}else
		{
			m_pOraQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"SimpleDbStatusChecker::Poll() Database returned empty result for database server  %s. Database status UNAVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
			m_reConnect = true;
		}
	}

	void SimpleDbStatusChecker::DoMysqlQuery()
		{
		TA_ASSERT(m_pMysqlConntion != NULL, "m_connection should not be NULL");
		if ( m_pMysqlConntion->isClosed() )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"SimpleDbStatusChecker::Poll() Database is not connected %s. Database status UNAVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
			m_reConnect = true;
			return;
		}

		int nReadOnly = 0;
		std::auto_ptr< sql::Statement > stmt(m_pMysqlConntion->createStatement());
		//m_pMysqlQuery = stmt->executeQuery("select count(*) from db_baseline_version");  // select version() cannot check the NDBCluster down
		m_pMysqlQuery = stmt->executeQuery("select @@read_only from dual");
		m_pMysqlConntion = stmt->getConnection();		
		if (m_pMysqlQuery->rowsCount() > 0)
		{
			m_pMysqlQuery->first();
			nReadOnly = m_pMysqlQuery->getInt(1);
		}

		if ( !nReadOnly )
		{			
			m_pMysqlQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"SimpleDbStatusChecker::Poll() Database poll successfull for database server %s. Database status AVAILABLE",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
			m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.

			return;
		}else
		{
			m_pMysqlQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"SimpleDbStatusChecker::Poll() Database returned read only for database server  %s. Database status UNAVAILABLE or Read Only",m_schemaName.c_str());
			setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
			m_reConnect = false; // if db just read only and alive, we do not need to re connect the db again.
		}
	}

	void SimpleDbStatusChecker::ParseDBType(const std::string& strDBName)
		{
		if(strDBName.compare(defOracleDBName) == 0) //case insensitive
		{
			m_nDBType = defOracleDBType;
		}
		else if (strDBName.compare(defMysqlDBName) == 0)
		{
			m_nDBType = defMysqlDBType;
	}
		else   
		{    // should never reach here, if reach here, then raise a exception
			LOG_GENERIC( SourceInfo, 
				TA_Base_Core::DebugUtil::DebugError,
				"the DB type string in database config file wrong(should be: Oracle or Mysql): %s" , strDBName.c_str());
			TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
		}
	}

	void SimpleDbStatusChecker::Init()
    {
		switch ( m_nDBType )
	{
		case defOracleDBType:
			m_pOraQuery    =  new OraQuery;
			OraEnvironment::setInitMode(OraEnvironment::imObject);
			OraEnvironment::setInitMode(OraEnvironment::imThreaded);
			OraEnvironment::setThreaded(true);
			m_pOraConnection = new ocl::OraConnection();		

			TA_ASSERT(NULL != m_pOraConnection, "Init():connection pointer is null.");
			m_pOraConnection->setConnectMode(cmNormal);
			m_pOraConnection->setUsername(m_strUserName); 
			m_pOraConnection->setPassword(m_dbPassword); 
			m_pOraConnection->setServer(m_schemaName);
			break;
		case defMysqlDBType:
			m_pMysqlDriver = (sql::Driver*)get_driver_instance();
			break;
	}

	}

    void SimpleDbStatusChecker::terminate()
    {
        m_run = false;
    }

	void SimpleDbStatusChecker::timerExpired(long timerId, void* userData)
	{
		if (m_dbStatusObserver)
			m_dbStatusObserver->notifyStatusChange(m_dbConnectionInfo, m_dbStatus);
	}

	void SimpleDbStatusChecker::setDatabaseOffLine()
	{
		setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
	}


}//namespace


