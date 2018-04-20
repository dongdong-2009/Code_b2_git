/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Chris DeWolfe
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last mofified by:  $Author$
  *
  * DbServerPoint class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/DbServerPoint.h"
#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"

#include "core/database/src/SimpleDb.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/database/src/DBException.h"

#include <sstream>
#include <time.h>
#include <string>

#ifdef WIN32
  #ifndef lint
    #define lint
  #endif
#endif

#include "core/ocl/src/ocl.h"

using namespace TA_Base_Core;
using namespace ocl;

namespace TA_Base_App
{

	std::vector<std::string> DbServerPoint::m_dbStatusNames;

	DbServerPoint::DbServerPoint
	( 
		TA_Base_Bus::BooleanDataPoint* const datapoint,
		const std::string& hostName
	)
		: m_datapoint(datapoint),
		  m_hostName(hostName),
		  m_dataBase(NULL),
		  m_dbTimeOutSec(10),
		  m_isUpdating(false),
          m_reConnect(true)
	{
		FUNCTION_ENTRY("DbServerPoint");
        m_query = new OraQuery;

        // dbStatus:<database>:<base database>
        m_baseServerName = m_hostName;
        size_t pos = m_hostName.find( ":" );

        if ( pos != std::string::npos )
        {
            std::string serverName = m_hostName.substr( 0, pos );
            std::string baseServerName = m_hostName.substr( pos + 1 );

            if ( false == serverName.empty() && false == baseServerName.empty() )
            {
                m_hostName = serverName;
                m_baseServerName = baseServerName;
            }
        }

        std::transform(m_baseServerName.begin(), m_baseServerName.end(), m_baseServerName.begin(), ::toupper);

		FUNCTION_EXIT;
	}



	DbServerPoint::~DbServerPoint()
	{
		FUNCTION_ENTRY("~DbServerPoint");
        delete m_query;
        detachDb();
		FUNCTION_EXIT;
	}

	ISelfResolvingPointContainer* DbServerPoint::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{

		FUNCTION_ENTRY("convertToIPointContainer");

		//see if this is a BooleanDataPoint:
		TA_Base_Bus::BooleanDataPoint* pBDP = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(dataPoint);

		if(0 == pBDP)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"convertToIPointContainer() refused non-BooleanDataPoint with pkey %u", dataPoint->getEntityKey());

			FUNCTION_EXIT;

			return 0;
		}

		//see if the address has the form "dbStatus:<host name>"

		std::string hostName = dataPoint->getDataPointAddress();

		//The true parameter in this call disables validation of the IP address portion of the string.
		//This is a good thing given that we're after the form dbStatus:<host name>
		if(!AbstractThreadedDriver::convertAddressFieldToIpAddress(hostName, "dbStatus:", true) )
		{
			FUNCTION_EXIT;

			return 0; //note that call already did logging so don't do it here
		}

		std::vector<std::string>::iterator findIt = std::find( m_dbStatusNames.begin(), m_dbStatusNames.end(), hostName ); 

		if (findIt != m_dbStatusNames.end())
		{
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "duplicated dbStatus configureation: %s, dbStatus Count=%d", 
				hostName.c_str(), m_dbStatusNames.size() );
			return 0;
		}
		else
		{
			m_dbStatusNames.push_back(hostName);
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "add dbStatus datapoint: %s, dbStatus Count=%d", 
				hostName.c_str(), m_dbStatusNames.size() );
		}

		DbServerPoint* newPoint = new DbServerPoint(pBDP, hostName);


		FUNCTION_EXIT;

		return newPoint;
	}

	TA_Base_Bus::DataPoint* const DbServerPoint::getDataPoint()
	{
		FUNCTION_ENTRY("getDataPoint");

		FUNCTION_EXIT;
		return m_datapoint;
	}

	std::string DbServerPoint::getAddressIdentifier()
	{
		FUNCTION_ENTRY("getAddressIdentifier");
		FUNCTION_EXIT;
		return m_hostName;
	}

    /** 
      * getConnectionParameters
      *
	  * retrieves the database schema name and password from the connection
	  * string supplied by the framework
      */
	void DbServerPoint::getConnectionParameters( std::string& schema, std::string& password )
	{
		FUNCTION_ENTRY("getConnectionParameters");
		
        std::string filename = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
		
        // can throw DbConnectionNoFile
        // no point in catching it cause there is nothing we can do with it
        TA_Base_Core::DbConnectionStrings connectStrings(filename);
        TA_Base_Core::DatabaseConnectionMap dbMap = connectStrings.getConnectionMap();
		
		
        TA_Base_Core::DatabaseConnectionMap::iterator it = dbMap.begin();
        while (it != dbMap.end() )
        {
            std::vector<TA_Base_Core::DataConnection>::iterator dcIt = it->second.begin();
            while (dcIt != it->second.end())
            {
                std::string dbServer = dcIt->first;
                std::transform( dbServer.begin(), dbServer.end(), dbServer.begin(), ::toupper );
                if ( dbServer == m_baseServerName )
				{
					std::string connection = dcIt->second;				

					int firstColonPos = connection.find(':');
					int secondColorPos = connection.rfind(':');
					
					schema = connection.substr(firstColonPos+1, secondColorPos-firstColonPos-1);
					password = connection.substr(secondColorPos+1,  connection.length());					

					return;
				}		
				
				dcIt++;
            }
			
			it++;
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"cant get schame and password for database %s.", m_hostName.c_str());

		FUNCTION_EXIT;
	}


	void DbServerPoint::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("onInit");

        //call this base class method so that the related ping point is resolved
        // AbstractPingDependentPoint::onInit(pEntityData); //limin-- CL-21689

		
		m_entityData = pEntityData;

		//Set the timeout
		m_dbTimeOutSec = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);

		//set the params as required
		getConnectionParameters( m_schemaName, m_dbPassword  );

		//removed this so that time is not taken to resolve the obj during init
		//resolve();

		FUNCTION_EXIT;
	}

    /*
	//This call throws if the db Server does not communicate
	void DbServerPoint::poll()
	{
		FUNCTION_ENTRY("poll");
		// API REF:
		//typedef std::vector<std::string> Row;
		//typedef std::vector<Row> Buffer;

		SimpleDb::Buffer data;

		//API REF:
		//bool Exec(const char* statement, int numColumns,
		//	SimpleDb::Buffer& data, int maxrows = SimpleDb::MAX_ROWS);
		try
		{
			m_dataBase->Exec("select 69 from dual", 1, data, 1);
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", e.getSummary().c_str() );
		}

		if( data.size() > 0 )
		{
			SimpleDb::Row row = data[0];

			std::string returnValue = row[0];

			//if we did not get the expected return value throw
			if( returnValue == "69" )
			{
        		FUNCTION_EXIT;
				return;
			}
		}

		FUNCTION_EXIT;
		throw "Can't get a 69 when required!";
	}
    */

    //Bohong++, replace the function commented above
    void DbServerPoint::poll()
    {
        FUNCTION_ENTRY("poll");
        
        try
        {
            m_query->setConnection(*m_dataBase);
            m_query->setCommandType(ocl::ctSQL);
            m_query->setCached(false);  
            m_query->setFetchRows(100);
            m_query->setSQL("select * from V$version");
            m_query->open();   
            m_dataBase = m_query->connection();
            if (m_query->isRowsReturn())
            {
                m_query->close();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "DbServerPoint::Poll() Database poll successfull for database server %s. Database status AVAILABLE",m_hostName.c_str());           
                FUNCTION_EXIT;
                return;
            }
            else
            {
                m_query->close();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "DbServerPoint::Poll() Database returned empty result for database server  %s. Database status UNAVAILABLE",m_hostName.c_str());
                m_reConnect = true;
            }
        }
        catch(ocl::OraException &e )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",
                m_hostName.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Database Error code %d. Reason: %s ; \n will re-connect database", 
                e.errorCode(), e.message().c_str());
            m_reConnect = true;
        }
        catch (CRException& )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",
                m_hostName.c_str());
            m_reConnect = true;
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "DbServerPoint::Poll() Caught exception while trying to connect to databse server %s.",
                m_hostName.c_str());
            m_reConnect = true;
        }
        
        FUNCTION_EXIT;
        throw "Can't get a reply when required!";
    }

	bool DbServerPoint::updatePointBasedOnProcessStatus()
	{
		FUNCTION_ENTRY("updatePointBasedOnProcessStatus");

		//We will set the point false if we can not resolve the db Server
		bool couldPoll = false;		
		
		try 			
		{
			//This call throws if the db Server does not communicate
			if (resolve())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Db Server resolved on %s", m_hostName.c_str());
				poll();
				//if it polled OK set the value to true
				couldPoll = true;
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Db Server poll() was OK on %s", m_hostName.c_str());
			}	
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "resolve database fail on host %s. Datapoint pkey = %u. Will set state to 0 (false).", m_hostName.c_str(), m_datapoint->getEntityKey());
                couldPoll = false;
                m_reConnect = true;
            }			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"threw when trying to retrieve status on host %s. Datapoint pkey = %u. Will set state to 0 (false).", m_hostName.c_str(), m_datapoint->getEntityKey());
			couldPoll = false;
            m_reConnect = true;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"finish resolve and poll of  datapoint (pkey = %u).", getDataPoint()->getEntityKey());

		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		//TD18334 diiDBSVR-DBStopRun DP value 0: database down; 1: database running;
		dpValue.setBoolean( couldPoll );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"Set datapoint (pkey = %u) state (%i, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey(), (int)couldPoll);


		FUNCTION_EXIT;

		return couldPoll;

	}

	void DbServerPoint::updateDataPoint()
	{
    	FUNCTION_ENTRY("updateDataPoint");
		m_isUpdating = true;
		try
		{
			unsigned long key = getDataPoint()->getEntityKey();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updating datapoint (pkey = %u).", key);

			updatePointBasedOnProcessStatus();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"updated datapoint (pkey = %u).", key);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Threw when trying to update datapoint (pkey = %u).", getDataPoint()->getEntityKey());
			//If here datapoint write threw (not the process resolution) , so forget about it

		}
		m_isUpdating = false;
		FUNCTION_EXIT;
	}

	bool DbServerPoint::isResolved()
	{
		//If not null it is resolved
		return ( m_dataBase != 0 );
	}

	bool DbServerPoint::isUpdating()
	{
		FUNCTION_ENTRY("isUpdating");
		FUNCTION_EXIT;
		return m_isUpdating;
	}
	/**
	* setStateForNoProcessResolution
	*
	* update the state of the data point to an appropriate state given that you can not read the status of the
	* related process.
	*
	* @throw if you can not write to the datapoint
	* 
	*/
	void DbServerPoint::setStateForNoProcessResolution()
	{
		FUNCTION_ENTRY("setStateForNoProcessResolution");

		//We will set the point false
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setBoolean( false );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Set datapoint (pkey = %u) state (false, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey());

		FUNCTION_EXIT;

		return;
	}

    /*
	bool DbServerPoint::resolve()
    {

        FUNCTION_ENTRY("resolve");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"resolve of datapoint %d gets called.",getDataPoint()->getEntityKey());

		if ( 0 != m_dataBase )
		{
			m_dataBase->CloseAll();
			delete m_dataBase;
			//This should wind up being 0 if the resolution fails
			m_dataBase = 0;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"delete db of datapoint %d.",getDataPoint()->getEntityKey());
		}

		//set the params as required. Note that for TransActive currently these params
		//should not be changing at runtime, hence this call is probably only required in
		//onInit(). But it is a low cost call so what the hell...

		if ( m_schemaName == "" || m_dbPassword == "")
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "database <%s> has invalid schemaName or password <%s>:<%s>, no need to poll.", 
					m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str());

			return false;
		}

	    try
	    {

			m_dataBase = new SimpleDbStatusMonitor( m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str() );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"create db of datapoint %d.",getDataPoint()->getEntityKey());

			//Now try and open the connection
			m_dataBase->Open();
        }
        catch ( ... ) //either the CTOR or open threw
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "exception caught while trying to create and open database with <%s>:<%s>:<%s>", 
				m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str());

			if ( 0 != m_dataBase )
			{
				delete m_dataBase;

				//This should wind up being 0 if the resolution fails
				m_dataBase = 0;
			}


			FUNCTION_EXIT;
			return false;
        }
		
        FUNCTION_EXIT;		
		return true;
    }
    */

    //Bohong++, replace the function commented above
	bool DbServerPoint::resolve()
    {
        FUNCTION_ENTRY("resolve");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"resolve of datapoint %d gets called.",getDataPoint()->getEntityKey());

        if ( !connect() )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "DbServerPoint::resolve() Caught exception while trying to connect to databse server %s schema %s password %s.",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str() );
            FUNCTION_EXIT;
            return false;
        }
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully opened database with <%s>:<%s>:<%s>", 
			m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str());
		
        FUNCTION_EXIT;		
		return true;
    }

    //Bohong++
    bool DbServerPoint::connect()
    {
        FUNCTION_ENTRY("connect");
        if (m_reConnect)
        {
            if ( m_schemaName == "" || m_dbPassword == "")
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "database <%s> has invalid schemaName or password <%s>:<%s>, no need to poll.", 
                    m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str());
                FUNCTION_EXIT
                return false;
            }
            
            try
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "DbServerPoint::resolve() create a new connection to database server %s...",m_hostName.c_str());
                OraEnvironment::setInitMode(OraEnvironment::imObject);
                OraEnvironment::setInitMode(OraEnvironment::imThreaded);
                OraEnvironment::setThreaded(true);
                m_dataBase = new ocl::OraConnection();
                m_dataBase->setConnectMode(cmNormal);
                m_dataBase->setUsername(m_schemaName); 
                m_dataBase->setPassword(m_dbPassword); 
                m_dataBase->setServer(m_hostName);
                m_dataBase->open();
            }
            catch(ocl::OraException & e)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "DbServerPoint::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. \nOCI_ERROR:[%s] ",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.message().c_str());
                detachDb();
                m_reConnect = true;
                FUNCTION_EXIT
                return false;
            }
            catch(std::exception & e)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "DbServerPoint::Connect() Caught exception while trying to connect to databse server %s: %s",m_hostName.c_str(), e.what());
                detachDb();
                m_reConnect = true;
                FUNCTION_EXIT
                return false;
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "DbServerPoint::Connect() Caught exception while trying to connect to databse server %s. Returning false",m_hostName.c_str());
                detachDb();
                m_reConnect = true;
                FUNCTION_EXIT
                return false;
            } 
            
            m_reConnect = false;
        }

        FUNCTION_EXIT
        return true;
    }

    //Bohong++
    void DbServerPoint::detachDb()
    {
        FUNCTION_ENTRY("detachDb");
        // When delete m_dataBase, OraConnection destructor will be called.
        // Normally, destructor should not throw any exception, or it will cause
        // the destructor incomplete. But actually when we disconnect db by cutting
        // off the lan cable, seems the destructor of OraConnection will throw
        // exception, that's terrible, I'm not sure this would cause some weird 
        // problem, even if adding a try-catch block here, because once an exception
        // is thrown by destructor that means the destructor is not finished.
        try
        {
            delete m_dataBase;
            m_dataBase = 0;
        }
        catch(ocl::OraException & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "delete fail server %s schema %s password %s. \nOCI_ERROR:[%s] ",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.message().c_str());
        }
        catch(std::exception & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "delete fail server %s schema %s password %s: %s",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.what());
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "delete fail server %s schema %s password %s.",m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str() );
        } 
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "delete db of datapoint %d.",getDataPoint()->getEntityKey());
        FUNCTION_EXIT;
    }

	void DbServerPoint::onlineConfigUpdate(std::vector<std::string> changedParams)
	{
        FUNCTION_ENTRY("onlineConfigUpdate");

		//See if the DB_SERVER_TIMEOUT_PARAM has been changed...
		std::vector<std::string>::iterator pString;

		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);

		//If the DB_SERVER_TIMEOUT_PARAM has been changed...
		if(pString != changedParams.end())
		{
			m_dbTimeOutSec = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);
			
		}

		FUNCTION_EXIT;
	}

} //end namespace TA_Base_App
