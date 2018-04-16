/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/SessionAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2011/05/12 17:46:29 $
  * Last modified by:  $Author: builder $
  * 
  * SessionAccessFactory is a singleton that is used to retrieve Session objects either from the
  * database or newly created. All Session objects returned will adhear to the ISession interface.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/Session.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
//#include <sstream>
#include <iomanip>

namespace TA_Base_Core
{
    SessionAccessFactory* SessionAccessFactory::m_instance = 0;
	std::string SessionAccessFactory::m_transactiveSuperSessionId = std::string("TransActive Super Session ID");

	const unsigned long SUPER_LOC_ID = 9997;
	const unsigned long MAX_LOC_ID = SUPER_LOC_ID+1;
	const unsigned long ERROR_LOC_ID = MAX_LOC_ID+1;

	const int MAX_LOC_ID_BIT = 4;

	//f550cfa6-d9c5-49dd-b336-0f747cd7ef5f	8-4-4-4-12 (36)
	const int UUID_FIRST_DASH_POS = 8;
	const int UUID_SECOND_DASH_POS = 13;
	const int UUID_THIRD_DASH_POS = 18;
	const int UUID_FOUTH_DASH_POS = 23;
	const int UUID_LENGTH = 36;


    SessionAccessFactory& SessionAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new SessionAccessFactory();
        }
        return *m_instance;
    }


    const std::string& SessionAccessFactory::getSuperSessionId()
    {
        return m_transactiveSuperSessionId;
    }


    ISession* SessionAccessFactory::getSession(const std::string& sessionId)
    {
        return new Session(sessionId);
    }

	std::vector<ISession*> SessionAccessFactory::getAllSessions(bool readWrite)
	{
		// Perform SQL here
		std::vector< ISession* > sessions;
		std::ostringstream sql;

		// create the SQL string to retrieve all the Subsystems of the alarm
        sql << "select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED from TA_SESSION";
		getSessions( sql.str(), sessions );
        
		return sessions;
	}

	std::vector<ISession*> SessionAccessFactory::getAllLocalSessions(bool readWrite, unsigned long locationId )
	{
		// Perform SQL here
		std::vector< ISession* > sessions;
		std::ostringstream sql;

		// create the SQL string to retrieve all the Subsystems of the alarm
        sql << "select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED from TA_SESSION where PKEY= ";
		sql << locationId;

		getSessions( sql.str(), sessions );
        
		return sessions;
	}

	std::vector<ISession*> SessionAccessFactory::getOwnedSessions(unsigned long entityKey)
	{
		// Perform SQL here
		std::vector< ISession* > sessions;
		std::ostringstream sql;

		sql << "select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED from TA_SESSION where ENTITY_ID = ";
		sql << entityKey;

		getSessions( sql.str(), sessions );
		return( sessions );
	}

	//chenlei++ CL-21183
	std::vector<ISession*> SessionAccessFactory::getOwnedActiveSessions(unsigned long entityKey)
	{
		// Perform SQL here
		std::vector< ISession* > sessions;
		std::ostringstream sql;
		
		sql << "select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED from TA_SESSION where HAS_ENDED = 0 and ENTITY_ID = ";
		sql << entityKey;
		
		getSessions( sql.str(), sessions );
		return( sessions );

	}
	//++chenlei

	ISession* SessionAccessFactory::createSession(const std::string& sessionID,
			const unsigned long operatorKey,
			const unsigned long consoleKey,
			const unsigned long profileKey,
			const unsigned long locationKey,
			const unsigned long entityKey)
	{
		return new Session( sessionID, operatorKey, consoleKey, profileKey, locationKey, entityKey, false, true );
		// false indicates that this is the START of a session
        // true indicates this is a new session that has to be written to the database
	}


    ISession* SessionAccessFactory::createSession(const unsigned long operatorKey,
			const unsigned long consoleKey,
			const unsigned long profileKey,
			const unsigned long locationKey,
			const unsigned long entityKey)
    {
        // Create the Session object to represent this object. Populate all the necessary data.
		// To write to the database it is simply a matter of calling apply changes().

		// The sessionID will be valid on the returned object
        return new Session( operatorKey, consoleKey, profileKey, locationKey, entityKey );
    }


	ISession* SessionAccessFactory::createEndedSession( const std::string& sessionId )
	{
		std::auto_ptr<ISession> session( new Session( sessionId ) );

		session->setHasEnded();
		return ( session.release() );
	}

    std::string SessionAccessFactory::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
                DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Sessions_Ad, Write);
                m_localDatabase = connections[0].first;  
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

    void SessionAccessFactory::deleteSession(ISession* session)
    {
        if (session == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Write);

        std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
	    formatSQL << "delete from TA_SESSION where PKEY = ''"<< session->getSessionId() <<"''";	
        formatSQL << "');";
        formatSQL << "END;";

        databaseConnection->executeModification(formatSQL.str());
    }

	void SessionAccessFactory::getSessions( const std::string& sql, std::vector<ISession*>& sessions )
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn     ("PKEY"       );
		std::string operatorColumn("OPERATORKEY");
        std::string consoleColumn ("CONSOLEKEY" );
		std::string profileColumn ("SEPROF_ID"  );
		std::string locationColumn("LOCATI_ID"  );
		std::string entityColumn  ("ENTITY_ID"  );
		std::string hasEndedColumn("HAS_ENDED"  );

        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn     );
		columnNames.push_back(operatorColumn);
		columnNames.push_back(consoleColumn );
		columnNames.push_back(profileColumn );
		columnNames.push_back(locationColumn);
		columnNames.push_back(entityColumn  );
		columnNames.push_back(hasEndedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            return;
        }
	
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    ISession* session = NULL;
                    session = new Session( data->getStringData( i, keyColumn ),
							               data->getUnsignedLongData(i, operatorColumn),
                                           data->getUnsignedLongData(i, consoleColumn ),
										   data->getUnsignedLongData(i, profileColumn ),
										   data->getUnsignedLongData(i, locationColumn),
										   data->getUnsignedLongData(i, entityColumn  ),
										   data->getBooleanData     (i, hasEndedColumn),
                                           false); // false indicates not a new session
					
                    sessions.push_back( session );
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
	}

	//TD18707 xinsong++
	std::string SessionAccessFactory::fillLocInfoToSessionId( unsigned long locationKey, const std::string & uuidString )
	{
		TA_Assert( locationKey >= 0);
		std::ostringstream ostr;
		ostr << "Location key should be lesser than " << locationKey;
		TA_ASSERT( locationKey <= MAX_LOC_ID, ostr.str().c_str() );

		//could not use clear
		ostr.str("");
		ostr << std::right << std::setfill('0');
		ostr << std::setw(MAX_LOC_ID_BIT) << locationKey;
		std::string locIdStr = ostr.str();
		TA_Assert( locIdStr.size() == MAX_LOC_ID_BIT );


		//f550cfa6-d9c5-49dd-b336-0f747cd7ef5f	8-4-4-4-12 (36)
		TA_Assert( uuidString[UUID_FIRST_DASH_POS] == '-' );
		TA_Assert( uuidString[UUID_SECOND_DASH_POS] == '-' );
		TA_Assert( uuidString[UUID_THIRD_DASH_POS] == '-' );
		TA_Assert( uuidString[UUID_FOUTH_DASH_POS] == '-' );
		
		std::string sessionId = uuidString;
		sessionId[UUID_FIRST_DASH_POS] = locIdStr[0];
		sessionId[UUID_SECOND_DASH_POS] = locIdStr[1];
		sessionId[UUID_THIRD_DASH_POS] = locIdStr[2];
		sessionId[UUID_FOUTH_DASH_POS] = locIdStr[3];
		return sessionId;
	}

	//if it is a superID, return SUPER_LOC_ID, if error, return ERROR_LOC_ID
	unsigned long SessionAccessFactory::getLocIDFromSessionId( const std::string & sessionId )
	{
		if ( isSupperSessionId(sessionId) )
		{
			return SUPER_LOC_ID;
		}
		if ( sessionId.length() != UUID_LENGTH )
		{
			return ERROR_LOC_ID;
		}

		char locIdBuffer[5];
		locIdBuffer[0] = sessionId[UUID_FIRST_DASH_POS];
		if ( ! isdigit(locIdBuffer[0]) )
		{
			return ERROR_LOC_ID;
		}
		locIdBuffer[1] = sessionId[UUID_SECOND_DASH_POS];
		if ( ! isdigit(locIdBuffer[1]) )
		{
			return ERROR_LOC_ID;
		}
		locIdBuffer[2] = sessionId[UUID_THIRD_DASH_POS];
		if ( ! isdigit(locIdBuffer[2]) )
		{
			return ERROR_LOC_ID;
		}
		locIdBuffer[3] = sessionId[UUID_FOUTH_DASH_POS];
		if ( ! isdigit(locIdBuffer[3]) )
		{
			return ERROR_LOC_ID;
		}

		locIdBuffer[4] = 0x0;
		return atoi(locIdBuffer);
	}

	bool SessionAccessFactory::isSupperSessionId( const std::string & sessionId )
	{
		return (sessionId==m_transactiveSuperSessionId);
	}
	
	bool SessionAccessFactory::isObsoleteSessionIdFormat( const std::string & sessionId )
	{
		if ( sessionId[UUID_FIRST_DASH_POS] == '-'  &&
		     sessionId[UUID_SECOND_DASH_POS] == '-' && 
			 sessionId[UUID_THIRD_DASH_POS] == '-'  && 
			 sessionId[UUID_FOUTH_DASH_POS] == '-' )
		{
			return true;
		}
		return false;
	}

	//return false if it is a super Id
	bool SessionAccessFactory::isCurrentSessionIdFormat( const std::string & sessionId )
	{
		unsigned long locID = getLocIDFromSessionId(sessionId);
		return (locID != ERROR_LOC_ID && locID != SUPER_LOC_ID );
	}

	bool SessionAccessFactory::isValidSessionId( const std::string & sessionId )
	{
		SESSION_FLAG sessionFlag;
		getSessionFlag( sessionId, sessionFlag );
		return sessionFlag != ERROR_SESSION_ID_FALG;
	}

	void SessionAccessFactory::getSessionFlag( const std::string & sessionId, SESSION_FLAG & sessionFlag )
	{
		if ( isSupperSessionId(sessionId) )
		{
			sessionFlag = SUPER_SESSION_ID_FALG;
			return;
		}
			
		if ( sessionId.size() < UUID_LENGTH )
		{
			sessionFlag = ERROR_SESSION_ID_FALG;
			return;
		}

		if ( isCurrentSessionIdFormat( sessionId ) )
		{
			sessionFlag = CURRENT_SESSION_ID_FALG;	
			return;
		}

		if ( isObsoleteSessionIdFormat( sessionId ) )
		{
			sessionFlag = OBSELETE_SESSION_ID_FALG;
			return;
		}


		sessionFlag = ERROR_SESSION_ID_FALG;
		return;
	}

    bool SessionAccessFactory::isSystemProfileSession(const std::string& sessionId)
    {
    	bool result = false;
		
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);
        std::string keyColumn     ("IS_SYSTEM");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        std::ostringstream sql;

        sql << "select sp.IS_SYSTEM from se_profile sp, ta_session se where se.pkey = '" << sessionId <<"' and se.seprof_id = sp.seprof_id";
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            
            return false;
        }
        
        if (data->getNumRows()==1)
        {
	        result = data->getBooleanData(0, keyColumn);
			
			delete data;
			data = NULL;
			
            return result;
        }
           
        return false;
    }

	bool SessionAccessFactory:: isEndedSession(const std::string& sessionId)
	{
		bool result = false;
		
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);
        std::string keyColumn     ("HAS_ENDED");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        std::ostringstream sql;

        sql << "select s.HAS_ENDED from ta_session s where s.pkey = '" << sessionId <<"' ";
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            
            return false;
        }
        
        if (data->getNumRows()==1)
        {
	        result = data->getBooleanData(0, keyColumn);
			
			delete data;
			data = NULL;
			
            return result;
        }

		delete data;
		data = NULL;

        return false;		
	}
    unsigned long SessionAccessFactory::getOperatorKey(const std::string& sessionId)
    {
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);
        std::string keyColumn     ("OPERATORKEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        
        std::ostringstream sql;
        
        sql << "select operatorkey from ta_session where pkey = '" << sessionId <<"'";
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            
            return false;
        }
        
        if (data->getNumRows()==1)
        {
            return data->getUnsignedLongData(0, keyColumn);
        }
        
        return false;
    }
    

} // closes TA_Base_Core
