/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/CombinedEventAccessFactory.cpp $
* @author:  Bradley Cleaver
* @version: $Revision: #3 $
*
* Last modification: $DateTime: 2011/01/24 12:13:56 $
* Last modified by:  $Author: builder $
*
* CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
* retrieve all data required to be presented to the user. It allows filtering and
* sorting to done on the database side, and returns a vector of partially loaded
* data to improve the efficiency.
*
*/

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/exceptions/src/DataException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;
using TA_Base_Core::Thread;

// Define all constants in an anonymous namespace.
namespace
{
	const unsigned long SOURCE_TABLE_EVENT( 100 );
	const unsigned long SOURCE_TABLE_DATAPOINT( 200 );

	const std::string COLUMN_SOURCE_TABLE( "SOURCE_TABLE" );
	const std::string COLUMN_PKEY( "PKEY" );
	const std::string COLUMN_CREATETIME( "CREATETIME" );
	const std::string COLUMN_CREATEDATETIME( "CREATEDATETIME" );
	const std::string COLUMN_SEVERITY_NAME( "SEVERITYNAME" );
	const std::string COLUMN_ASSET_NAME( "ASSETNAME" );
	const std::string COLUMN_DESCRIPTION( "DESCRIPTION" );
	const std::string COLUMN_EVENT_TYPE_NAME( "EVENT_TYPE_NAME" );
	const std::string COLUMN_VALUE( "VALUE" );
	const std::string COLUMN_MMS( "MMS" );
	const std::string COLUMN_DSS( "DSS" );
	const std::string COLUMN_AVL( "AVL" );
	const std::string COLUMN_OPERATOR_NAME( "OPERATORNAME" );
	const std::string COLUMN_ALARM_COMMENT( "ALARMCOMMENT" );
	const std::string COLUMN_ALARM_ID( "ALARM_ID" );
	const std::string COLUMN_ALARM_ACK( "ALARM_ACK" );
	const std::string COLUMN_ALARM_STATUS( "ALARM_STATUS" );
	const std::string COLUMN_MAX_EVENT_KEY( "MaxEventKey" );
	const std::string COLUMN_MAX_DATAPOINT_KEY( "MaxDatapointKey" );

	const std::string COLUMN_LATEST_HOUR( "LatestHour" );
	const std::string ORDER_ASCENDING( "ASC" );
	const std::string ORDER_DESCENDING( "DESC" );

	const std::string ALWAYS_FALSE( "ALWAYS FALSE" );
	const std::string COLUMN_MAX_EVENT_LEVEL ( "MaxEventLevel");
	const std::string COLUMN_EVENT_COUNT ( "EventCount" );
};

namespace TA_Base_Core
{
	CombinedEventAccessFactory* CombinedEventAccessFactory::m_instance = NULL;

	CombinedEventAccessFactory& CombinedEventAccessFactory::getInstance()
	{
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new CombinedEventAccessFactory();
		}

		return *m_instance;
	}

	unsigned long  CombinedEventAccessFactory::getMaxEventPKey()
	{

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		// Create the SQL statement to execute.
		std::string sql("SELECT last_number as maxpkey, cache_size as cachesize From user_sequences where upper(sequence_name) in ('EV_COMBINED_SEQ')");
		// Set up the columnNames vector to be passed to executeQuery()
		std::string maxPkeyColumn = "maxpkey";
		std::string cacheSizeColumn = "cachesize";
		std::vector<std::string> columnNames;
		columnNames.push_back( maxPkeyColumn );
		columnNames.push_back( cacheSizeColumn );
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );
		unsigned long maxpkey= data->getUnsignedLongData(0 , maxPkeyColumn );
		unsigned long cacheSize= data->getUnsignedLongData(0 , cacheSizeColumn );
		cacheSize=cacheSize*2+1;
		std::stringstream sqlbuffer;
		columnNames.clear();
		columnNames.push_back( maxPkeyColumn );
		sqlbuffer<<"select /*+  index (ev_combined EVCOM_PKEY_INDX ) */  max(pkey) as maxpkey from ev_combined where pkey between "<< maxpkey-cacheSize << " and " <<maxpkey + cacheSize << " order by Pkey desc" ;
		std::auto_ptr<IData> data2( databaseConnection->executeQuery( sqlbuffer.str(), columnNames ) );
		if( 0 < data2->getNumRows())
		{
			maxpkey= data2->getUnsignedLongData(0 , maxPkeyColumn );
		}
		else
		{
          return maxpkey;
		}
		return maxpkey;
	}


	time_t CombinedEventAccessFactory::getEarliestEventTime()
	{
		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Create the SQL statement to execute.
		std::string sql("SELECT TO_CHAR( MIN(createtime),'YYYYMMDDHH24MISS') AS earliesttime FROM ev_combined where createtime<sysdate");

		// Set up the columnNames vector to be passed to executeQuery()
		std::string earliestTimeColumn = "EARLIESTTIME";
		std::vector<std::string> columnNames;
		columnNames.push_back( earliestTimeColumn );

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );

		return data->getDateData( 0, earliestTimeColumn );
	}

	std::vector<ICombinedEventData*> CombinedEventAccessFactory::loadAllEvents( const EventFilter& filter)
	{
		// Now populate the vector with the matching events.
		std::vector<ICombinedEventData*> loadedEvents;

		if ( filterExcludesAll( filter, false ) )
		{
			m_lastWhereClause = ALWAYS_FALSE;
			return loadedEvents;
		}

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Generate the where clause.
		// Add the time filters
		// Generate the where clause.
		m_lastWhereClause = generateWhereClause( filter, "", *databaseConnection );

		std::ostringstream timeRestrictClause;

		// Add the time filters
		std::string toTime;
		if ( 0 != filter.toTime )
		{
			const unsigned int bufLen = 15;
			char buf[bufLen];
			struct tm tmToTime = *localtime(&filter.toTime);

			strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

			toTime = buf;
			toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";
		}		

		std::string fromTime;
		if ( 0 == filter.fromTime )
		{
			// Default is 24 hours before to.
			fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
		}
		else
		{
			const unsigned int bufLen = 15;
			char buf[bufLen];
			struct tm tmFromTime = *localtime(&filter.fromTime);

			strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

			fromTime = buf;
			fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
		}

		// Now construct the time portion of the where clause.
		timeRestrictClause << "createtime >= " << fromTime;
		if ( !toTime.empty() )
		{
			timeRestrictClause << " AND createtime <= " << toTime;
		}

		std::ostringstream eventPkeySql;
		std::ostringstream datapointPkeySql;

		// Get the latest audit pkey from ev_combined
		unsigned long maxPkey=0;
		// Get the latest audit pkey from ev_combined
		maxPkey=getMaxEventPKey();
		if (maxPkey>m_curMaxPkey)
		{
			m_curMaxPkey=maxPkey;
		}		
// 		eventPkeySql<<"SELECT MAX(pkey) From ev_combined WHERE"<<" source_table='100' AND "<<timeRestrictClause.str();
// 		datapointPkeySql<<"SELECT MAX(pkey) From ev_combined WHERE"<<" source_table='200' AND "<<timeRestrictClause.str();
// 
// 		std::vector<std::string> eventPkeyColumnNames;
// 		eventPkeyColumnNames.push_back( COLUMN_MAX_EVENT_KEY );
// 		std::auto_ptr<IData> eventPkeydata( databaseConnection->executeQuery( eventPkeySql.str(), eventPkeyColumnNames ) );
// 		if( 0 < eventPkeydata->getNumRows())
// 		{
// 			try
// 			{
// 				unsigned long latestAuditPkey = eventPkeydata->getUnsignedLongData(0, COLUMN_MAX_EVENT_KEY);
// 				if( m_latestAuditPkey < latestAuditPkey)
// 				{
// 					m_latestAuditPkey = latestAuditPkey;
// 					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The maximum audit key of this load is %d)", m_latestAuditPkey);
// 				}
// 			}
// 			catch(TA_Base_Core::DataException& ex)
// 			{
// 				// Most likely, no audit events in database.
// 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
// 			}
// 		}

// 		// fetch the latest data point key from ev_combined
// 		std::vector<std::string> datapointPkeyColumnNames;
// 		datapointPkeyColumnNames.push_back( COLUMN_MAX_DATAPOINT_KEY );
// 		std::auto_ptr<IData> datapointPkeydata( databaseConnection->executeQuery( datapointPkeySql.str(), datapointPkeyColumnNames ) );
// 		if( 0 < datapointPkeydata->getNumRows())
// 		{
// 			try
// 			{
// 				unsigned long latestDatapointPkey = datapointPkeydata->getUnsignedLongData(0, COLUMN_MAX_DATAPOINT_KEY);
// 				if( m_latestDatapointPkey < latestDatapointPkey )
// 				{
// 					m_latestDatapointPkey = latestDatapointPkey;
// 					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The maximum datapoint key of this load is %d)", m_latestDatapointPkey);
// 				}
// 			}
// 			catch(TA_Base_Core::DataException& ex)
// 			{
// 				// Most likely, no audit events in database.
// 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
// 			}
// 		}

		// wenguang++ TD14188
		std::ostringstream selectSql;

		selectSql<< " SELECT ev_combined.source_table AS source_table,"
			<< " ev_combined.pkey AS pkey,"
			<< " TO_CHAR(ev_combined.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
			<< " ev_combined.severity_name AS severityname,"
			<< " ev_combined.asset_name AS assetname,"
			<< " ev_combined.description AS description,"
			<< " ev_combined.event_type_name AS event_type_name,"
			<< " ev_combined.value AS value,"
			<< " ev_combined.mms_state AS mms,"
			<< " ev_combined.dss_state AS dss,"
			<< " ev_combined.avl_state AS avl,"
			<< " ev_combined.operator_name AS operatorname,"
			<< " ev_combined.alarm_comment AS alarmcomment,"
			<< " ev_combined.alarm_id AS alarm_id,"
			<< " ev_combined.alarm_ack AS alarm_ack,"
			<< " ev_combined.alarm_status AS alarm_status"
			<< " FROM ev_combined";

		if (m_lastWhereClause.empty() == false)
		{
			selectSql << " WHERE " << m_lastWhereClause;
		}

		std::ostringstream sql;
		sql << selectSql.str(); 

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );

		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );

		if ( 0 < data->getNumRows() )
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
			}
		}

		return loadedEvents;
	}






	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getFirstEventsOnPage( const EventFilter& filter,
		const unsigned long pageNumber, const unsigned long eventsPerPage )
	{
		// Now populate the vector with the matching events.
		std::vector<ICombinedEventData*> loadedEvents;

		// If the filter excludes all possible events just return.
		if ( filterExcludesAll( filter, false ) )
		{
			m_lastWhereClause = ALWAYS_FALSE;
			return loadedEvents;
		}

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Paging is ignored for now... just get first 500 rows.
		unsigned long pageStart = (pageNumber*eventsPerPage)+1;
		unsigned long pageEnd = pageStart+eventsPerPage-1;

		// Generate the where clause.
		m_lastWhereClause = generateWhereClause( filter,"", *databaseConnection );

// 		// SQL statement to fetch the biggest event pkey
// 		std::ostringstream eventPkeySql;
// 		eventPkeySql << "(SELECT MAX(pkey) FROM ev_combined WHERE source_table = 100) AS \""
// 			<< COLUMN_MAX_EVENT_KEY << "\"";
// 
// 		// SQL statement to fetch the biggest datapoint pkey
// 		std::ostringstream datapointPkeySql;
// 		datapointPkeySql << "(SELECT MAX(pkey) FROM ev_combined WHERE source_table <> 100) AS \""
// 			<< COLUMN_MAX_DATAPOINT_KEY << "\"";

		unsigned long maxPkey=0;
		// Get the latest audit pkey from ev_combined
		maxPkey=getMaxEventPKey();
		if (maxPkey>m_curMaxPkey)
		{
			m_curMaxPkey=maxPkey;
		}	

		// This is the SQL statement that will perform the filtering and sorting.
		std::ostringstream internalSql;
		internalSql << "SELECT " // BACTODO: cache hint goes here.
			<< "ev_c.source_table AS source_table, "
			<< "ev_c.pkey AS pkey, "
			<< "TO_CHAR(ev_c.createtime,'YYYYMMDDHH24MISSFF3') AS createtime, "
			<< "ev_c.severity_name AS severityname, "
			<< "ev_c.asset_name AS assetname, "
			<< "ev_c.description AS description, "
			<< "ev_c.event_type_name AS event_type_name,"
			<< "ev_c.value AS value,"
			<< "ev_c.mms_state AS mms, "
			<< "ev_c.dss_state AS dss, "
			<< "ev_c.avl_state AS avl, "
			<< "ev_c.operator_name AS operatorname, "
			<< "ev_c.alarm_comment AS alarmcomment, "
			<< "ev_c.alarm_id AS alarm_id, "
			<< "ev_c.alarm_ack AS alarm_ack, "
			<< "ev_c.alarm_status AS alarm_status "
			<< "FROM ev_combined ev_c "
			<< "WHERE " << m_lastWhereClause << " "
			<< "ORDER BY " << determineSortColumn( filter ) << " " << determineSortOrder( filter );

		// In order to retrieve pages we need to associate row numbers and retrieve ranges based of these.
		std::ostringstream sql;
		sql << "SELECT source_table, pkey, createtime, severityname, assetname, description, event_type_name,  value, mms, dss, avl, "
			<< "operatorname, alarmcomment, alarm_id, alarm_ack, alarm_status "
// 			<< eventPkeySql.str() << ", "
// 			<< datapointPkeySql.str()
			<< " FROM "
			<< "( SELECT ROWNUM AS rn, data.* FROM ( " << internalSql.str() << " ) data ) "
			<< "WHERE rn BETWEEN " << pageStart << " AND " << pageEnd;

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_MAX_EVENT_KEY );
		columnNames.push_back( COLUMN_MAX_DATAPOINT_KEY );

		// Execute the SQL.
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );

		if ( 0 < data->getNumRows() )
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
			}
// 			// Every row in the query will contain the maximum audit event and datapoint
// 			// event keys. Just use row 0.
// 			try
// 			{
// 				unsigned long latestAuditPkey = data->getUnsignedLongData(0, COLUMN_MAX_EVENT_KEY);
// 				if( latestAuditPkey > m_curMaxPkey)
// 				{
// 					m_curMaxPkey = latestAuditPkey;
// 				}
// 			}
// 			catch(TA_Base_Core::DataException& ex)
// 			{
// 				// Most likely, no audit events in database.
// 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
// 			}
// 
// 			try
// 			{
// 				unsigned long latestDatapointPkey = data->getUnsignedLongData(0, COLUMN_MAX_DATAPOINT_KEY);
// 				if( latestDatapointPkey > m_curMaxPkey)
// 				{
// 					m_curMaxPkey = latestDatapointPkey;
// 				}
// 			}
// 			catch(TA_Base_Core::DataException& ex)
// 			{
// 				// Most likely, no datapoint events in database.
// 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
// 			}
		}
		else
		{
			//m_latestAuditPkey = 0;
			//m_latestDatapointPkey = 0;			
		}

		return loadedEvents;
	}


	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getNextEvents()
	{
		std::vector<ICombinedEventData*> loadedEvents;

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		IData* dataPtr = NULL;
		if ( databaseConnection->moreData( dataPtr ) && ( 0 < dataPtr->getNumRows() ) )
		{
			// Take care of memory management.
			std::auto_ptr<IData> data( dataPtr );

			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
				unsigned long pkey=data->getUnsignedLongData(i,COLUMN_PKEY);
				if (pkey>m_curMaxPkey) m_curMaxPkey=pkey;
			}
		}

		return loadedEvents;
	}


	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getEventsSinceLastLoad()
	{
		std::vector<ICombinedEventData*> newEvents;

		// If the filter excludes all possible events just return.
		if ( ALWAYS_FALSE == m_lastWhereClause )
		{
			return newEvents;
		}

		// First the event table needs to be checked for new elements.
		LatestKeys latest = getLatestPkeys();

		// If the latest pkeys match ours then don't load. The keys will never match exactly, rather
		// they are considered to match if the latest is exactly one ahead of the last loaded.
		if ( ( latest.maxPkey <= m_curMaxPkey  ))
		{
			return newEvents;
		}

		// Set the default set operations.
// 		std::string auditSetOp = "AND";
// 		std::string datapointSetOp = "AND";
// 
// 		if ( latest.maxPkey < m_curMaxPkey )
// 		{
// 			// If the pkey has wrapped around the set operation is different.
// 			auditSetOp = "OR";
// 		}
// 		if ( latest.maxPkey < m_curMaxPkey )
// 		{
// 			// If the pkey has wrapped around the set operation is different.
// 			datapointSetOp = "OR";
// 		}

        std::stringstream  sql;		 
		{
		// Now generate the query.
			
			sql << "SELECT /*+  index (ev_combined EVCOM_PKEY_INDX ) */  ev_c.source_table AS source_table, ev_c.pkey AS pkey, "
				<< "TO_CHAR(ev_c.createtime,'YYYYMMDDHH24MISSFF3') AS createtime, "
				<< "ev_c.severity_name AS severityname, ev_c.asset_name AS assetname, "
				<< "ev_c.description AS description, ev_c.event_type_name AS event_type_name, ev_c.value AS value, "
				<< "ev_c.mms_state AS mms, ev_c.dss_state AS dss, ev_c.avl_state AS avl, "
				<< "ev_c.operator_name AS operatorname, ev_c.alarm_comment AS alarmcomment, "
				<< "ev_c.alarm_id AS alarm_id, "
				<< "ev_c.alarm_ack AS alarm_ack, "
				<< "ev_c.alarm_status AS alarm_status "
				<< "FROM ev_combined ev_c "
				<< "WHERE ( ( ev_c.pkey > " << m_curMaxPkey
				<< " " << " and " << " ev_c.pkey <= " << latest.maxPkey << " ) ) ) AND "
				<< m_lastWhereClause;
		}

// 			<< "WHERE (( ev_c.source_table = '100' AND ( ev_c.pkey > " << m_curMaxPkey
// 			<< " " << auditSetOp << " ev_c.pkey <= " << latest.auditPkey
// 			<< " ) ) OR ( ev_c.source_table = '200' AND ( ev_c.pkey > " << m_curMaxPkey
// 			<< " " << datapointSetOp << " ev_c.pkey <= " << latest.datapointPkey << " ) ) ) AND "
			

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );

		// Execute the SQL.
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );

		m_curMaxPkey = latest.maxPkey;
		//m_latestDatapointPkey = latest.datapointPkey;

		for( unsigned int i=0; i< data->getNumRows(); i++ )
		{
			ICombinedEventData* event = createCombinedEvent( i, data.get() );


			// Now construct the object and add it to the list.
			newEvents.push_back( event );
		}

		return newEvents;
	}


	void CombinedEventAccessFactory::resetStopFlag()
	{
		m_stopLoad = false;
	}

	void CombinedEventAccessFactory::applicationTerminating()
	{
		m_stopLoad = true;
		m_pauseLoad = false;
	}


	void CombinedEventAccessFactory::applicationPausing()
	{
		m_pauseLoad = true;
	}


	void CombinedEventAccessFactory::applicationResuming()
	{
		m_pauseLoad = false;
	}


	ICombinedEventData* CombinedEventAccessFactory::createCombinedEvent( unsigned long row, IData* data )
	{
		// Get the source table ID and convert it to an event type.
		ECombinedEventType eventType = ( SOURCE_TABLE_EVENT == data->getUnsignedLongData( row, COLUMN_SOURCE_TABLE ) ) ? E_AUDIT_EVENT : E_DPSTATECHANGE_EVENT;

		// Get the pkey for the event.
		unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );

		// Get the time the event was created.
		timeb createTime = data->getTimestampData( row, COLUMN_CREATETIME );

		// Get the severity name.
		std::string severityName = data->getStringData( row, COLUMN_SEVERITY_NAME );

		// Get the asset name.
		std::string assetName = data->getStringData( row, COLUMN_ASSET_NAME );

		// Get the description.
		std::string description = data->getStringData( row, COLUMN_DESCRIPTION );
		std::string eventTypeName = data->getStringData( row, COLUMN_EVENT_TYPE_NAME );
		// TD 14751 get the expanded description with event type appended to front
		std::string expandedDescription("");
		if (eventTypeName.empty() == false) expandedDescription = eventTypeName + "- ";
		expandedDescription += description;

		// Get the value.
		std::string value = data->getStringData( row, COLUMN_VALUE );

		// Get the MMS state information.
		std::string mmsState = data->getStringData( row, COLUMN_MMS );

		// Get the DSS state information.
		std::string dssState = data->getStringData( row, COLUMN_DSS );

		// Get the AVL state information.
		std::string avlState = data->getStringData( row, COLUMN_AVL );

		// Get the operator name.
		std::string operatorName = data->getStringData( row, COLUMN_OPERATOR_NAME );

		// Get the alarm comment.
		std::string alarmComment = data->getStringData( row, COLUMN_ALARM_COMMENT );

		// Get the alarm ID.
		std::string alarmId = data->getStringData( row, COLUMN_ALARM_ID );
		if ( alarmId == " " )
		{
			alarmId = "";
		}

		// Get the alarm ack.
		std::string alarmAck = data->getStringData( row, COLUMN_ALARM_ACK );
		if ( alarmAck == " " )
		{
			alarmAck = "";
		}

		// Get the alarm status.
		std::string alarmStatus = data->getStringData( row, COLUMN_ALARM_STATUS );
		if ( alarmStatus == " " )
		{
			alarmStatus = "";
		}
		return new CombinedEventData( eventType, pkey, createTime, severityName, assetName, expandedDescription,
			value, mmsState, dssState, avlState, operatorName, alarmComment, alarmId, alarmAck, alarmStatus );
	}


	CombinedEventAccessFactory::LatestKeys CombinedEventAccessFactory::getLatestPkeys()
	{
		LatestKeys keys;		 
		try
		{
			 
			keys.maxPkey= getMaxEventPKey();
		}
		catch ( ... )
		{
		}
		return keys;
	}


	const std::string& CombinedEventAccessFactory::determineSortColumn( const EventFilter& filter )
	{
		// Determine what column will be sorted on based off the order enumeration.
		switch ( filter.sortColumn )
		{
		case SORT_CREATETIME:
			{
				return COLUMN_CREATEDATETIME;
				break;
			}

		case SORT_SEVERITY:
			{
				return COLUMN_SEVERITY_NAME;
				break;
			}

		case SORT_ASSET:
			{
				return COLUMN_ASSET_NAME; 
				break;
			}

		case SORT_DESCRIPTION:
			{
				return COLUMN_DESCRIPTION; 
				break;
			}

		case SORT_VALUE:
			{
				return COLUMN_VALUE; 
				break;
			}

		case SORT_MMS:
			{
				return COLUMN_MMS; 
				break;
			}

		case SORT_DSS:
			{
				return COLUMN_DSS; 
				break;
			}

		case SORT_AVL:
			{
				return COLUMN_AVL; 
				break;
			}

		case SORT_OPERATOR:
			{
				return COLUMN_OPERATOR_NAME; 
				break;
			}

		case SORT_COMMENT:
			{
				return COLUMN_ALARM_COMMENT; 
				break;
			}
		}

		// An unknown order was supplied? Assume default.
		return COLUMN_CREATETIME;
	}

	const std::string& CombinedEventAccessFactory::determineSortOrder( const EventFilter& filter )
	{
		// Determine the order in which the sort should be applied.
		if ( SORT_ASCENDING == filter.sortOrder )
		{
			return ORDER_ASCENDING;
		}

		return ORDER_DESCENDING;
	}

	std::string CombinedEventAccessFactory::generateWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection )
	{
		std::stringstream whereClause;

		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;

		if( strcmp(timeRestrictionClause.c_str(), "") == 0 )
		{
			// Add the time filters
			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.toTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				toTime = buf;
				toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";
			}

			std::string fromTime;
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.fromTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				fromTime = buf;
				fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
			}

			// Now construct the time portion of the where clause.
			whereClause << "createtime >= " << fromTime;
			if ( !toTime.empty() )
			{
				whereClause << " AND createtime <= " << toTime;
			}
		}
		else
		{
			whereClause << timeRestrictionClause;
		}

		// Add the event level.
		whereClause << " AND event_level != 0 AND event_level <= " << filter.eventLevel; 

		// Add filters for locations and subsystems.
		std::string locAndSubsystem = generateLocAndSubsystemExpr( filter, databaseConnection );
		if ( !locAndSubsystem.empty() )
		{
			whereClause << " AND ( " << locAndSubsystem << " )";
		}

		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "event_type_key", filter.eventTypes, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		whereClause << " AND ( ";
		if ( filter.showNonAlarmEvents )
		{
			whereClause << " severity_key IS NULL or severity_key=0 ";
		}
		if ( !filter.severityIds.empty() )
		{
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " OR ";
			}
			whereClause << generateORList( "severity_key", filter.severityIds, databaseConnection );
		}
		whereClause << " )";
 




		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}

		return whereClause.str();
	}

	std::string CombinedEventAccessFactory::generateORList( const std::string& columnName,
		const NumericList& keyList,
		IDatabase& databaseConnection)
	{
		// The string to store the list in.
		std::stringstream expression;

		NumericList::const_iterator keyIt;
		for ( keyIt=keyList.begin(); keyIt!=keyList.end(); keyIt++ )
		{
			// If this isn't the first element then it needs an OR added first..
			if ( keyIt != keyList.begin() )
			{
				expression << " OR ";
			}

			// Add the expression to the list.
			expression << databaseConnection.escapeInsertString(columnName) << "=" << *keyIt;
		}

		return expression.str();
	}


	std::string CombinedEventAccessFactory::generateLocAndSubsystemExpr( const EventFilter& filter, IDatabase& databaseConnection )
	{
		//if ( !filter.dutyEnabled )
		{
			// Duty isn't enabled.
			// ( loc1 OR loc2 ) AND ( ss1 OR ss2 OR pss1 OR pss2 )

			// Get the expression that filters on location.
			std::string locationExpr = generateORList( "location_key", filter.locationIds, databaseConnection );
			
			std::string subsystemExpr = joinExpressions(
				generateORList( "subsystem_key", filter.appSubsystemIds, databaseConnection ), "OR",
				generateORList( "physical_subsystem_key", filter.physSubsystemIds, databaseConnection ), false );
			
			// Return the expression.
			return joinExpressions( locationExpr, "AND", subsystemExpr );
		}
// 		else if ( filter.physSubsystemIds.empty() && filter.appSubsystemIds.empty() )
// 		{
// 			// No physical or application subsystems were specified - All subsystes.
// 			// [{duty expr} OR] ( pss IS NULL [AND ( loc1 OR loc2 )] )
// 
// 			// Get the expression that filters based on duty.
// 			std::string dutyExpr = generateDutyExpr( filter.duty, databaseConnection );
// 
// 			// Get the expression that filters on location.
// 			std::string locationExpr = joinExpressions( "physical_subsystem_key IS NULL", "AND",
// 				generateORList( "location_key", filter.locationIds, databaseConnection ) );
// 
// 			// Return the expression.
// 			return joinExpressions( dutyExpr, "OR", locationExpr );
// 		}
// 		else if ( filter.physSubsystemIds.empty() && !filter.appSubsystemIds.empty() )
// 		{
// 			// No physical, but application subsystems specified - Only application.
// 			// [( loc1 OR loc2 ) AND] ( ss1 OR ss2 )
// 
// 			// Get the expression that filters based on duty.
// 			std::string locationExpr = generateORList( "location_key", filter.locationIds, databaseConnection );
// 
// 			// Get the expression that filters on application subsystem.
// 			std::string appSubsystemExpr = generateORList( "subsystem_key", filter.appSubsystemIds, databaseConnection );
// 
// 			// Return the expression.
// 			return joinExpressions( locationExpr, "AND", appSubsystemExpr );
// 		}
// 		else if ( !filter.physSubsystemIds.empty() && filter.appSubsystemIds.empty() )
// 		{
// 			// Physical, but no application subsystems specified - Only physical.
// 			// {duty expr}
// 			return generateDutyExpr( filter.duty, databaseConnection );
// 		}
// 		else if ( !filter.physSubsystemIds.empty() && !filter.appSubsystemIds.empty() )
// 		{
// 			// Physical and application subsystems specified - Both application and physical.
// 			// [{duty expr} OR] ( [( loc1 OR loc2 ) AND] ( ss1 OR ss2 ) )
// 
// 			// Get the expression that filters based on duty.
// 			std::string dutyExpr = generateDutyExpr( filter.duty, databaseConnection );
// 
// 			// Get the expression that filters on location.
// 			std::string locationExpr = generateORList( "location_key", filter.locationIds, databaseConnection );
// 
// 			// Get the expression that filters on application subsystem.
// 			std::string appSubsystemExpr = generateORList( "subsystem_key", filter.appSubsystemIds, databaseConnection );
// 
// 			// Return the expression.
// 			return joinExpressions( dutyExpr, "OR", joinExpressions( locationExpr, "AND", appSubsystemExpr ) );
// 		}

		return "";
	}


	std::string CombinedEventAccessFactory::generateFullDutyExpr( const FullDutyMap& duty, IDatabase& databaseConnection )
	{
		std::stringstream expression;

		// Select group of locations.
		FullDutyMap::const_iterator dutyIt;
		for ( dutyIt=duty.begin(); dutyIt!=duty.end(); dutyIt++ )
		{
			// If its not the first item it needs an OR.
			if ( dutyIt!=duty.begin() )
			{
				expression << " OR ";
			}
			else
			{
				expression<<"(";
			}
			// Add the location key.
			expression << "((";

			// Now add the physical subsystems visible for that location.

			SubSysList::const_iterator keyIt;
			for ( keyIt = dutyIt->second.begin(); keyIt != dutyIt->second.end(); keyIt++ )
			{
				// If this isn't the first element then it needs an OR added first..
				if ( keyIt != dutyIt->second.begin() )
				{
					expression << " OR ";
				}

				// Add the expression to the list.
				expression << "( ev_combined.physical_subsystem_key =" << (*keyIt).physicalSubsystemId << " AND "
					<<"se_profile_access.sessta_id"<< "=" << (*keyIt).physicalSubsystemState << " )" ;
			}

			expression << " ) AND " <<"ev_combined.location_key=" << dutyIt->first <<" )";
		}
		expression<<")";
		return expression.str();
	}
	std::string CombinedEventAccessFactory::generateDutyExpr( const NumericMap& duty, IDatabase& databaseConnection )
	{
		std::stringstream expression;

		// Select group of locations.
		NumericMap::const_iterator dutyIt;
		for ( dutyIt=duty.begin(); dutyIt!=duty.end(); dutyIt++ )
		{
			// If its not the first item it needs an OR.
			if ( dutyIt!=duty.begin() )
			{
				expression << " OR ";
			}

			// Add the location key.
			expression << "( location_key=" << dutyIt->first;

			// Now add the physical subsystems visible for that location.
			std::string subsystems = generateORList( "physical_subsystem_key", dutyIt->second, databaseConnection );
			expression << " AND ( " << subsystems << " ) )";
		}

		return expression.str();
	}

	std::string CombinedEventAccessFactory::joinExpressions( const std::string& expr1,
		const std::string& operation,
		const std::string& expr2,
		bool bracketExpr /*=true*/ )
	{
		std::stringstream joined;

		if ( !expr1.empty() )
		{
			if ( bracketExpr )
			{
				joined << "( " << expr1 << " )";
			}
			else
			{
				joined << expr1;
			}
		}
		if ( !expr1.empty() && !expr2.empty() )
		{
			joined << " " << operation << " ";
		}
		if ( !expr2.empty() )
		{
			if ( bracketExpr )
			{
				joined << "( " << expr2 << " )";
			}
			else
			{
				joined << expr2;
			}
		}

		return joined.str();
	}

	bool CombinedEventAccessFactory::filterExcludesAll( const EventFilter& filter, bool isVisibilityEnabled)
	{
		bool returnVal = false;

		// If no severites or 'non alarm events'...
		if ( filter.severityIds.empty() && !filter.showNonAlarmEvents )
		{
			returnVal = true;
		}

		// If duty is enabled AND no duty exists AND no application subsystems are selected while
		// some physical are...
// 		else if ( !isVisibilityEnabled && filter.dutyEnabled && filter.duty.empty() && !filter.physSubsystemIds.empty() &&
// 			filter.appSubsystemIds.empty() )
// 		{
// 			returnVal = true;
// 		}
// 		else if( isVisibilityEnabled && filter.dutyEnabled && filter.fullDuty.empty() && !filter.physSubsystemIds.empty() &&
// 			filter.appSubsystemIds.empty() )
// 		{
// 			returnVal = true;
// 		}

		return returnVal;
	}

	std::string CombinedEventAccessFactory::generateDutyEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause ,TA_Base_Core::IDatabase& databaseConnection )
	{
		std::stringstream whereClause;

		whereClause<< " FROM se_action_group_map, se_action_group, se_profile_access, ev_combined, se_action WHERE ";

		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;

		// Add the time filters 
		if( strcmp( timeRestrictionClause.c_str(), "" ) == 0 )
		{
			// if the timeRestrictionClause is empty string
			// generate the time restriction base on the information in the filter

			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.toTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				toTime = buf;
				toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";
			}

			std::string fromTime;
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.fromTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				fromTime = buf;
				fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
			}

			// Now construct the time portion of the where clause.
			whereClause << "ev_combined.createtime >= " << fromTime;
			if ( !toTime.empty() )
			{
				whereClause << " AND ev_combined.createtime <= " << toTime;
			}
		}
		else
		{
			whereClause << timeRestrictionClause;
		}
		// Add the restriction to retrieve physical subsystem events 
		whereClause << " AND "<< generateFullDutyExpr( filter.fullDuty, databaseConnection);

		whereClause << " AND se_action_group_map.seagro_id = se_action_group.seagro_id"
			<< " AND se_action_group_map.seacti_id = se_action.seacti_id"
			<< " AND se_action_group.seagro_id = se_profile_access.seagro_id";

		if( !filter.profiles.empty())
		{
			whereClause<<" AND ("<<generateORList("se_profile_access.seprof_id", filter.profiles, databaseConnection)<<")";
		}

		whereClause << " AND ev_combined.physical_subsystem_key = se_profile_access.subsystemkey"
			<< " AND ev_combined.event_type_key = se_action.memtyp_id";

		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.event_type_key", filter.eventTypes, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND ev_combined.asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(ev_combined.asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( ev_combined.description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(ev_combined.description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		whereClause << " AND ( ";
		if ( filter.showNonAlarmEvents )
		{
			whereClause << " ev_combined.severity_key IS NULL";
		}
		if ( !filter.severityIds.empty() )
		{
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " OR ";
			}
			whereClause << generateORList( "ev_combined.severity_key", filter.severityIds, databaseConnection );
		}
		whereClause << " )";

		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (ev_combined.alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR ev_combined.alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND ev_combined.alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}


		// Add the event level.
		whereClause << " AND ev_combined.event_level != 0 AND ev_combined.event_level <= " << filter.eventLevel; 

		return whereClause.str();
	}

	std::string CombinedEventAccessFactory::generateOwnerEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection )
	{
		std::stringstream whereClause;

		whereClause<< " FROM ev_combined, ta_session session1, ta_session session2 WHERE ";
		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;

		// Add the time filters

		if( strcmp(timeRestrictionClause.c_str(), "") == 0 )
		{
			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.toTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				toTime = buf;
				toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";	
			}

			std::string fromTime;
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.fromTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				fromTime = buf;
				fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
			}

			// Now construct the time portion of the where clause.
			whereClause << "ev_combined.createtime >= " << fromTime;
			if ( !toTime.empty() )
			{
				whereClause << " AND ev_combined.createtime <= " << toTime;
			}
		}
		else
		{
			whereClause << timeRestrictionClause;
		}


		// Add the restriction to retrieve operator owned events
		whereClause << " AND session1.pkey = '" << databaseConnection.escapeInsertString(filter.sessionId)<<"'"
			<<" AND ev_combined.physical_subsystem_key IS NULL";

		if( !filter.profiles.empty())
		{
			whereClause << " AND ("<<generateORList("session2.seprof_id", filter.profiles, databaseConnection) <<")";
		}


		whereClause <<" AND session1.locati_id = session2.locati_id"
			<<" AND ev_combined.session_key = session2.pkey";

		// Add the location and subsystem.
		std::string locAppSubsystemStr=generateLocAndAppSubsystemExpr(filter, databaseConnection);
		if( !locAppSubsystemStr.empty())
		{
			whereClause << " AND ("<<locAppSubsystemStr<<")";
		}

		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.event_type_key", filter.eventTypes, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND ev_combined.asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(ev_combined.asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( ev_combined.description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(ev_combined.description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		whereClause << " AND ( ";
		if ( filter.showNonAlarmEvents )
		{
			whereClause << " ev_combined.severity_key IS NULL";
		}
		if ( !filter.severityIds.empty() )
		{
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " OR ";
			}
			whereClause << generateORList( "ev_combined.severity_key", filter.severityIds, databaseConnection );
		}
		whereClause << " )";

		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (ev_combined.alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR ev_combined.alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND ev_combined.alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}

		// Add the event level.
		whereClause << " AND ev_combined.event_level != 0 AND ev_combined.event_level <= " << filter.eventLevel; 

		return whereClause.str();
	}

	std::string CombinedEventAccessFactory::generateSystemEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection )
	{
		std::stringstream whereClause;

		whereClause << " FROM se_action_group_map, se_action_group, se_profile_access, se_action, ev_combined WHERE ";
		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;

		// Add the time filters

		if( strcmp( timeRestrictionClause.c_str(), "") == 0)
		{
			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.toTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				toTime = buf;
				toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";
			}

			std::string fromTime;
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.fromTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				fromTime = buf;
				fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
			}

			// Now construct the time portion of the where clause.
			whereClause << "ev_combined.createtime >= " << fromTime;
			if ( !toTime.empty() )
			{
				whereClause << " AND ev_combined.createtime <= " << toTime;
			}
		}
		else
		{
			whereClause << timeRestrictionClause;
		}


		// Add the restriction to retrieve system generated events which current profile have rights to view
		whereClause << " AND se_action_group.seagro_id = se_action_group_map.seagro_id"
			<< " AND se_action_group.seagro_id = se_profile_access.seagro_id";

		if(!filter.profiles.empty())
		{
			whereClause << " AND ("<<generateORList("se_profile_access.seprof_id", filter.profiles, databaseConnection)<<")";
		}

		whereClause << " AND ev_combined.physical_subsystem_key IS NULL"
			<< " AND ( ev_combined.session_key IS NULL or exists(select 1 from operator where pkey in (select operator_key from ta_session where pkey = ev_combined.session_key) and is_system=1))"
			<< " AND ev_combined.subsystem_key = se_profile_access.subsystemkey"
			<< " AND ev_combined.event_type_key = se_action.memtyp_id"
			<< " AND se_action_group_map.seacti_id = se_action.seacti_id";

		// Add the location and subsystem.
		std::string locAppSubsystemStr=generateLocAndAppSubsystemExpr(filter, databaseConnection);
		if( !locAppSubsystemStr.empty())
		{
			whereClause << " AND ("<<locAppSubsystemStr<<")";
		}

		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.event_type_key", filter.eventTypes, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND ev_combined.asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(ev_combined.asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( ev_combined.description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(ev_combined.description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		whereClause << " AND ( ";
		if ( filter.showNonAlarmEvents )
		{
			whereClause << " ev_combined.severity_key IS NULL";
		}
		if ( !filter.severityIds.empty() )
		{
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " OR ";
			}
			whereClause << generateORList( "ev_combined.severity_key", filter.severityIds, databaseConnection );
		}
		whereClause << " )";

		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev_combined.alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (ev_combined.alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR ev_combined.alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND ev_combined.alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}

		// Add the event level.
		whereClause << " AND ev_combined.event_level != 0 AND ev_combined.event_level <= " << filter.eventLevel; 


		return whereClause.str();
	}

	std::string CombinedEventAccessFactory::generateLocAndAppSubsystemExpr( const EventFilter& filter, IDatabase& databaseConnection )
	{
		// This part is only used in retrieving operator owned events and system generated events

		if ( !filter.appSubsystemIds.empty() && !filter.locationIds.empty() )
		{
			// application subsystems specified - Only application.
			// ( loc1 OR loc2 ) AND ( ss1 OR ss2 )

			// Get the expression that filters based on location.
			std::string locationExpr = generateORList( "ev_combined.location_key", filter.locationIds, databaseConnection );

			// Get the expression that filters on application subsystem.
			std::string appSubsystemExpr = generateORList( "ev_combined.subsystem_key", filter.appSubsystemIds, databaseConnection );

			// Return the expression.
			return joinExpressions( locationExpr, "AND", appSubsystemExpr );
		}
		else if( !filter.locationIds.empty() )
		{
			// Get the expression that filters based on location.
			std::string locationExpr = generateORList( "ev_combined.location_key", filter.locationIds, databaseConnection );
			// Return the expression.
			std::stringstream returnstr;
			returnstr<<"( "<< locationExpr <<" )"; 
			return returnstr.str();
		}
		else if( !filter.appSubsystemIds.empty() )
		{
			// Get the expression that filters on application subsystem.
			std::string appSubsystemExpr = generateORList( "ev_combined.subsystem_key", filter.appSubsystemIds, databaseConnection );
			// Return the expression.
			std::stringstream returnstr;
			returnstr<<"( "<< appSubsystemExpr <<" )"; 
			return returnstr.str();
		}

		return "";
	}

	std::vector<ICombinedEventData*> CombinedEventAccessFactory::loadDesignatedEvents(bool enableVisibilitRule,  const EventFilter& filter, const std::string pageStartTime, const unsigned long endRow)
	{
		// Now populate the vector with the matching events.
		std::vector<ICombinedEventData*> loadedEvents;

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		if ( filter.dutyEnabled == false )
		{
			m_isLoadingAll = true;
			return loadAllEvents(filter);
		}
		else
		{
			m_isLoadingAll = false;
		}

		// If the filter excludes all possible events just return.
		if ( filterExcludesAll( filter, true ) )
		{
			m_lastDutyWhereClause = ALWAYS_FALSE;
			m_lastOwnerWhereClause = ALWAYS_FALSE;
			m_lastSystemWhereClause = ALWAYS_FALSE;
			return loadedEvents;
		}
         

		std::ostringstream selectSql;
		std::ostringstream sql;

		if (!enableVisibilitRule)
		{
			m_lastWhereClause = generateWhereClause(filter, "", *databaseConnection);
				selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				<< " source_table AS source_table,"
				<< " pkey AS pkey,"
				<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " severity_name AS severityname,"
				<< " asset_name AS assetname,"
				<< " description AS description,"
				<< " event_type_name AS event_type_name,"
				<< " value AS value,"
				<< " mms_state AS mms,"
				<< " dss_state AS dss,"
				<< " avl_state AS avl,"
				<< " operator_name AS operatorname,"
				<< " alarm_comment AS alarmcomment,"
				<< " alarm_id AS alarm_id,"
				<< " alarm_ack AS alarm_ack,"
		    	<< " alarm_status AS alarm_status"
		    	<< " FROM ev_combined";		
		     	  
			    sql << selectSql.str()  <<" WHERE " << m_lastWhereClause;
				 
		}
		else
		{
			m_lastWhereClause = generateDesignatedWhereClause(filter, "", *databaseConnection);
			selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				<< " ev.source_table AS source_table,"
				<< " ev.pkey AS pkey,"
				<< " TO_CHAR(ev.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " ev.severity_name AS severityname,"
				<< " ev.asset_name AS assetname,"
				<< " ev.description AS description,"
				<< " ev.event_type_name AS event_type_name,"
				<< " ev.value AS value,"
				<< " ev.mms_state AS mms,"
				<< " ev.dss_state AS dss,"
				<< " ev.avl_state AS avl,"
				<< " ev.operator_name AS operatorname,"
				<< " ev.alarm_comment AS alarmcomment,"
				<< " ev.alarm_id AS alarm_id,"
				<< " ev.alarm_ack AS alarm_ack,"
			    << " ev.alarm_status AS alarm_status";
			
			
				sql << selectSql.str() << m_lastWhereClause;
		}

		

		bool isloadLatestRecord=false;
		if ( 0 == filter.fromTime  && 0==filter.toTime ) 
		{
			isloadLatestRecord=true;
			m_curMaxPkey=0;
		}

// 		if (isloadLatestRecord)
// 		{
// 			unsigned long maxPkey=0;
// 			// Get the latest audit pkey from ev_combined
// 			maxPkey=getMaxEventPKey();
// 			if (maxPkey>m_curMaxPkey)
// 			{
// 				m_curMaxPkey=maxPkey;
// 			}			 
// 		}

		

	

	
		if( pageStartTime != "")
		{
			sql << " AND ev.createtime <= TO_TIMESTAMP('" << pageStartTime << "', 'YYYYMMDDHH24MISS') ";
		}
        
// 		if (isloadLatestRecord)
// 		{
// 			sql << " AND ev.pkey > " << m_curMaxPkey-endRow << " and ev.pkey<" << m_curMaxPkey;
// 			m_nextPkey=m_curMaxPkey;
// 		}

	    sql << " AND ROWNUM  <= " << endRow << " order by createtime desc";

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );

		for (unsigned long sqlCnt=0; sqlCnt<sql.str().size()/100+1;sqlCnt++)
		{
			std::string newstr=sql.str().substr(sqlCnt*100,100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}

		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );


		if ( 0 < data->getNumRows() )
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
				unsigned long pkey=data->getUnsignedLongData(i,COLUMN_PKEY);			
				if (pkey>m_curMaxPkey) m_curMaxPkey=pkey;				
			}
		}
		else		 
		{
			unsigned long maxPkey=0;
			// Get the latest audit pkey from ev_combined
			maxPkey=getMaxEventPKey();
			if (maxPkey>m_curMaxPkey)
			{
				m_curMaxPkey=maxPkey;
			}			 
		}

		
		 

		return loadedEvents;
	}

 
	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getDesignatedEventsSinceLastLoad(bool enableVisibilitRule)
	{
		std::vector<ICombinedEventData*> newEvents;

		if( m_isLoadingAll)
		{
			return getEventsSinceLastLoad();
		}

		// If the filter excludes all possible events just return.
		if ( ALWAYS_FALSE == m_lastWhereClause)
		{
			return newEvents;
		}

		// First the event table needs to be checked for new elements.
		LatestKeys latest = getLatestPkeys();

		// If the latest pkeys match ours then don't load. The keys will never match exactly, rather
		// they are considered to match if the latest is exactly one ahead of the last loaded.
		//if ( ( latest.auditPkey == m_latestAuditPkey ) && ( latest.datapointPkey == m_latestDatapointPkey ) )
		if ( ( latest.maxPkey <= m_curMaxPkey ) )
		{
			return newEvents;
		}

		std::stringstream auditKeyRestrictionClause;
		std::stringstream dpKeyRestrictionClause;

 
	
		std::ostringstream selectSql;
		std::stringstream  sql;
		if (!enableVisibilitRule)
		{		 
			auditKeyRestrictionClause<<" AND (pkey>" << m_curMaxPkey
				<< " and" << " pkey<=" << latest.maxPkey <<")";

			selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				<< " source_table AS source_table,"
				<< " pkey AS pkey,"
				<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " severity_name AS severityname,"
				<< " asset_name AS assetname,"
				<< " description AS description,"
				<< " event_type_name AS event_type_name,"
				<< " value AS value,"
				<< " mms_state AS mms,"
				<< " dss_state AS dss,"
				<< " avl_state AS avl,"
				<< " operator_name AS operatorname,"
				<< " alarm_comment AS alarmcomment,"
				<< " alarm_id AS alarm_id,"
				<< " alarm_ack AS alarm_ack,"
				<< " alarm_status AS alarm_status"
				<< " FROM ev_combined  "
				<< "WHERE ";			 			
		}
		else
		{
		 
			auditKeyRestrictionClause<<" AND (ev.pkey>" << m_curMaxPkey
				<< " and" << " ev.pkey<=" << latest.maxPkey <<")";

		selectSql<< " SELECT  /*+  index (ev_combined EVCOM_PKEY_INDX ) */  ev.source_table AS source_table,"
				<< " ev.pkey AS pkey,"
				<< " TO_CHAR(ev.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " ev.severity_name AS severityname,"
				<< " ev.asset_name AS assetname,"
				<< " ev.description AS description,"
				<< " ev.event_type_name AS event_type_name,"
				<< " ev.value AS value,"
				<< " ev.mms_state AS mms,"
				<< " ev.dss_state AS dss,"
				<< " ev.avl_state AS avl,"
				<< " ev.operator_name AS operatorname,"
				<< " ev.alarm_comment AS alarmcomment,"
				<< " ev.alarm_id AS alarm_id,"
				<< " ev.alarm_ack AS alarm_ack,"
				<< " ev.alarm_status AS alarm_status";
			
		}
         sql << selectSql.str() << m_lastWhereClause << auditKeyRestrictionClause.str()  << " and rownum<=2000 order by  createtime desc" ;
		 
		 
		

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		for (unsigned long sqlCnt=0; sqlCnt<sql.str().size()/100+1;sqlCnt++)
		{
			std::string newstr=sql.str().substr(sqlCnt*100,100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}

		// Execute the SQL.
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );

		//m_latestAuditPkey = latest.auditPkey;
		//m_latestDatapointPkey = latest.datapointPkey;

		for( unsigned int i=0; i< data->getNumRows(); i++ )
		{
			ICombinedEventData* event = createCombinedEvent( i, data.get() );

			// Now construct the object and add it to the list.
			newEvents.push_back( event );
		}

// 		if(data->getNumRows()==0)
// 		{
			if (  latest.maxPkey> m_curMaxPkey )
			{
				 m_curMaxPkey=latest.maxPkey;
			}		
// 
// 		}

		return newEvents;
	}
	std::string CombinedEventAccessFactory::generateDesignatedDutyExpr( const FullDutyMap& duty, IDatabase& databaseConnection, bool haveAppSubsystem )
	{
		std::stringstream expression;

		// Select group of locations.
		FullDutyMap::const_iterator dutyIt;
		for ( dutyIt=duty.begin(); dutyIt!=duty.end(); dutyIt++ )
		{
			// If its not the first item it needs an OR.
			if ( dutyIt!=duty.begin() )
			{
				expression << " OR ";
			}
			else
			{
				expression<<"(";
			}
			// Add the location key.
			expression << "((";

			// Now add the physical subsystems visible for that location.

			SubSysList::const_iterator keyIt;
			for ( keyIt = dutyIt->second.begin(); keyIt != dutyIt->second.end(); keyIt++ )
			{
				// If this isn't the first element then it needs an OR added first..
				if ( keyIt != dutyIt->second.begin() )
				{
					expression << " OR ";
				}

				// Add the expression to the list.
				expression << "( ev.physical_subsystem_key =" << (*keyIt).physicalSubsystemId << " AND "
					<<"t6.sessta_id"<< "=" << (*keyIt).physicalSubsystemState << " )" ;
			}

			if(haveAppSubsystem)
			{
				expression << "OR (ev.physical_subsystem_key = -1 AND t6.sessta_id=0) ";
			}

			expression << " ) AND " <<"ev.location_key=" << dutyIt->first <<" )";
		}
		expression<<")";
		return expression.str();
	}

	std::string CombinedEventAccessFactory::generateDesignatedWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause ,TA_Base_Core::IDatabase& databaseConnection )
	{
		std::stringstream whereClause;

		std::stringstream timeString;
		timeString<<timeRestrictionClause;

		std::string  acsString="";
		
		acsString=" ( select unique subsystemkey,physical_subsystem_key, seacti_id from t6 where t6.sessta_id=0  ";
		
		
		if( !filter.profiles.empty())
		{
			acsString=acsString + "AND (" + generateORList("t6.seprof_id", filter.profiles, databaseConnection) + ")";
		}
		
		acsString=acsString + " ) ";
		
		whereClause<< " FROM ev_combined ev, "<<  acsString.c_str()  << " t8 WHERE ";

		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;

		// Add the time filters 
		if( strcmp( timeRestrictionClause.c_str(), "" ) == 0 )
		{
			// if the timeRestrictionClause is empty string
			// generate the time restriction base on the information in the filter

			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.toTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				toTime = buf;
				toTime = "TO_TIMESTAMP('" + toTime + "','DDMMYYYYHH24MISS')";
			}

			std::string fromTime="";
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				 fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.fromTime);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				fromTime = buf;
				fromTime = "TO_TIMESTAMP('" + fromTime + "','DDMMYYYYHH24MISS')";
			}

			// Now construct the time portion of the where clause.
			if ( !fromTime.empty() )
			{			
				timeString << "ev.createtime >= " << fromTime;
			}
			if ( !toTime.empty() )
			{
				if (!fromTime.empty())
				{
					timeString << " AND ev.createtime <= " << toTime;
				}
				else
				{
					timeString << "  ev.createtime <= " << toTime;
				}
				
			}
		}
		 
		// Add the restriction to retrieve physical subsystem events 
		//disable duty function
		// 		if(filter.dutyEnabled && !filter.fullDuty.empty())
		// 		{
		// 			whereClause << " AND "<< generateDesignatedDutyExpr( filter.fullDuty, databaseConnection, !(filter.appSubsystemIds.empty() && !filter.physSubsystemIds.empty()));
		// 		}else{
		// 			whereClause << " AND ev.physical_subsystem_key = -1 AND t6.sessta_id=0 ";
		// 		}

		//disable duty function

	 
        std::string str=timeString.str();

        if (str.size()>0)
		{
            whereClause << str;
		}

		if (str.size() >0)
		{
			whereClause << " AND ev.action_id = t8.seacti_id";
		}
		else
		{
			whereClause << " ev.action_id = t8.seacti_id";
		}

		whereClause << " AND ev.physical_subsystem_key = t8.physical_subsystem_key"
			<< " AND ev.subsystem_key = t8.subsystemkey";

		if( !filter.locationIds.empty())
		{
			whereClause<<" AND ("<<generateORList("ev.location_key", filter.locationIds, databaseConnection)<<")";
		}	 

        if (!(filter.appSubsystemIds.empty()))
		{			 
	         
			 if (!(filter.physSubsystemIds.empty()))
			 {			 
				 whereClause << " AND (  ("<<generateORList("ev.subsystem_key", filter.appSubsystemIds, databaseConnection)<<" )" ;
				 whereClause << " OR  ("<<generateORList("ev.physical_subsystem_key", filter.physSubsystemIds, databaseConnection) <<" ) )" ;
				 
			 }else{
                 whereClause << " AND  ("<<generateORList("ev.subsystem_key", filter.appSubsystemIds, databaseConnection)<<" )" ;
			 }

		}else if(!(filter.physSubsystemIds.empty())){
		       whereClause << " AND  ("<<generateORList("ev.physical_subsystem_key", filter.physSubsystemIds, databaseConnection) <<" )" ;
		}


		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev.operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev.event_type_key", filter.eventTypes, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND ev_combined.asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(ev.asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( ev_combined.description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(ev.description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		if(!filter.showNonAlarmEvents || filter.severityIds.size() != 4)
		{
			whereClause << " AND ( ";
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " ev.severity_key IS NULL or ev.severity_key=0 ";
			}
			if ( !filter.severityIds.empty() )
			{
				if ( filter.showNonAlarmEvents )
				{
					whereClause << " OR ";
				}
				whereClause << generateORList( "ev.severity_key", filter.severityIds, databaseConnection );
			}
			whereClause << " )";
		}

		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev.alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (ev.alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR ev.alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND ev.alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}


		// Add the event level.
		std::string evLevelSql = "select max(view_level) view_level from me_message_type";
		unsigned long maxViewLevelCount;
		std::vector<std::string> maxViewLevel;
		maxViewLevel.push_back( COLUMN_MAX_EVENT_LEVEL );
		std::auto_ptr<IData> maxViewLevelData( databaseConnection.executeQuery( evLevelSql, maxViewLevel ) );
		if( 0 < maxViewLevelData->getNumRows())
		{
			try
			{ 
				maxViewLevelCount = maxViewLevelData->getUnsignedLongData(0, COLUMN_MAX_EVENT_LEVEL);
			}
			catch(TA_Base_Core::DataException& ex)
			{
				// Most likely, no audit events in database.
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
			}
		}
		whereClause << " AND ev.event_level != 0 "; 

		if(filter.eventLevel<maxViewLevelCount)
		{
			whereClause << " AND ev.event_level <= " << filter.eventLevel; 
		}

		return whereClause.str();
	}

};
