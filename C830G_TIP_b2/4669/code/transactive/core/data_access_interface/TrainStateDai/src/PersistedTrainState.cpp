/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "core/data_access_interface/TrainStateDai/src/PersistedTrainState.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DataConversion.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>

namespace TA_IRS_Core
{
    
    const std::string PersistedTrainState::TABLE_NAME = "TR_TRAIN"; 
    const std::string PersistedTrainState::TRAIN_ID_COLUMN = "TRAIN_ID";
    const std::string PersistedTrainState::PRIMARY_TSI_COLUMN = "PRIMARY_TSI";
    const std::string PersistedTrainState::SECONDARY_TSI_COLUMN = "SECONDARY_TSI";
    const std::string PersistedTrainState::PA_LIBRARY_VERSION_COLUMN = "PA_LIBRARY_VERSION";
    const std::string PersistedTrainState::TTIS_LIBRARY_VERSION_COLUMN = "TTIS_LIBRARY_VERSION";
	const std::string PersistedTrainState::NEXT_TTIS_LIBRARY_VERSION_COLUMN = "NEXT_TTIS_LIBRARY_VERSION";
    const std::string PersistedTrainState::TTIS_SCHEDULE_VERSION_COLUMN = "TTIS_SCHEDULE_VERSION";
	const std::string PersistedTrainState::NEXT_TTIS_SCHEDULE_VERSION_COLUMN = "NEXT_TTIS_SCHEDULE_VERSION";
    const std::string PersistedTrainState::TIMESTAMP_COLUMN = "TIMESTAMP";

    TA_Base_Core::NonReEntrantThreadLockable PersistedTrainState::s_columnNamesLock;
    PersistedTrainState::ColumnNames PersistedTrainState::s_columnNames;


    PersistedTrainState::~PersistedTrainState()
    {
        FUNCTION_ENTRY("~PersistedTrainState");
        FUNCTION_EXIT;
    }


    PersistedTrainState::PersistedTrainState( TA_Base_Core::IData& data,
                                              unsigned int row )
      : m_trainId( 0 ),
        m_primaryTsi( "" ),
        m_secondaryTsi( "" ),
        m_paLibraryVersion( 0 ),
        m_ttisLibraryVersion( 0 ),
        m_ttisScheduleVersion( 0 ),
        m_nextttisLibraryVersion( 0 ),
        m_nextttisScheduleVersion( 0 ),
        m_timestamp( 0 )
    {
        FUNCTION_ENTRY("PersistedTrainState");

        // load the data
        loadData(data, row);

        FUNCTION_EXIT;
    }


    const IPersistedTrainStateMap PersistedTrainState::loadAllTrainStates()
    {
        FUNCTION_ENTRY("loadAllTrainStates");
        
        // creates a query to load all data from the database.
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Read );


        // The query should use the constants
        // defined in this class, and it should
        // use the getColumnNames method provided.
        std::ostringstream sql;
        sql << "SELECT "
            << TRAIN_ID_COLUMN << ", "
            << PRIMARY_TSI_COLUMN << ", "
            << SECONDARY_TSI_COLUMN << ", "
            << PA_LIBRARY_VERSION_COLUMN << ", "
            << TTIS_LIBRARY_VERSION_COLUMN << ", "
            << TTIS_SCHEDULE_VERSION_COLUMN<< ", "
            << NEXT_TTIS_LIBRARY_VERSION_COLUMN << ", "
            << NEXT_TTIS_SCHEDULE_VERSION_COLUMN << ", "
            << "TO_CHAR( " << TIMESTAMP_COLUMN << ", 'YYYYMMDDHH24MISS' ) || '000' AS " << TIMESTAMP_COLUMN
            << " FROM " << TABLE_NAME;

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
  
        
        // When the query returns, the IData
        // object can be used to create the
        // PersistedTrainState objects, one per row.
        // These can be mapped on train ID   
        IPersistedTrainStateMap persistTrainStateMap;

        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), getColumnNames() );
                 
        do
        {
            // for each row
            for ( unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                // extract the row into an object
                // catch each one individually, because one failure should not stop the rest loading
                try
                {
                    // extract the row into an object
                    IPersistedTrainStatePtr iPersistedTrainStatePtr( new PersistedTrainState(*data, i) );
                    persistTrainStateMap.insert( IPersistedTrainStateMap::value_type( iPersistedTrainStatePtr->getTrainId(), iPersistedTrainStatePtr ) );
                }
                catch ( TA_Base_Core::DataException& de )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
                }
                catch ( TA_Base_Core::DatabaseException& dbe )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
                }
            }
            
            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
        return persistTrainStateMap;
    }


    void PersistedTrainState::saveTrainState( unsigned char trainId,
                                    const std::string& primaryTsi,
                                    const std::string& secondaryTsi,
                                    unsigned short paLibraryVersion,
                                    unsigned short ttisLibraryVersion,                                    
                                    unsigned short ttisTimeSchedule,
                                    unsigned short nextttisLibraryVersion,
                                    unsigned short nextttisTimeSchedule,
                                    time_t timestamp )
    {
        FUNCTION_ENTRY("saveTrainState");

		try {

			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection =
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Write );

			// call the stored procedure TRAIN.TR_TRAIN_UPDATE
			// in the database toperform the insert/update
			std::ostringstream sql;
			sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
				<< "BEGIN TRAIN.TR_TRAIN_UPDATE( "
				<< static_cast< unsigned int>( trainId ) << ", ''"
				<< primaryTsi << "'', ''"
				<< secondaryTsi << "'', "
				<< paLibraryVersion << ", "
				<< ttisLibraryVersion << ", "
				<< ttisTimeSchedule << ", "
                << timestamp << ", "
				<< nextttisLibraryVersion << ", "
				<< nextttisTimeSchedule << ")"
				<< ";END;'); END;"
				<< std::ends;

			// stored procedure execution
			databaseConnection->executeProcedure( sql.str() );
		}
		catch ( TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
        }
        catch ( TA_Base_Core::DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
        }
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", "UNKNOWN" );
		}

        FUNCTION_EXIT;
    }


    const PersistedTrainState::ColumnNames& PersistedTrainState::getColumnNames()
    {
        FUNCTION_ENTRY("getColumnNames");

        // acquires the s_columnNamesLock
        TA_THREADGUARD(s_columnNamesLock);

        // if the s_columnNames vector is empty,
        // populate it with all column names
        if( true == s_columnNames.empty() )
        {
            s_columnNames.push_back( TRAIN_ID_COLUMN );
            s_columnNames.push_back( PRIMARY_TSI_COLUMN );
            s_columnNames.push_back( SECONDARY_TSI_COLUMN );
            s_columnNames.push_back( PA_LIBRARY_VERSION_COLUMN );
            s_columnNames.push_back( TTIS_LIBRARY_VERSION_COLUMN );
            s_columnNames.push_back( TTIS_SCHEDULE_VERSION_COLUMN );
			s_columnNames.push_back( NEXT_TTIS_LIBRARY_VERSION_COLUMN );
			s_columnNames.push_back( NEXT_TTIS_SCHEDULE_VERSION_COLUMN );
            s_columnNames.push_back( TIMESTAMP_COLUMN );
        }

        FUNCTION_EXIT;   
        return s_columnNames;
    }


    const std::string& PersistedTrainState::getPrimaryTsi()
    {
        FUNCTION_ENTRY("getPrimaryTsi");
        FUNCTION_EXIT;
        return m_primaryTsi;
    }


    unsigned char PersistedTrainState::getTrainId()
    {
        FUNCTION_ENTRY("getTrainId");
        FUNCTION_EXIT;
        return m_trainId;
    }


    const std::string& PersistedTrainState::getSecondaryTsi()
    {
        FUNCTION_ENTRY("getSecondaryTsi");
        FUNCTION_EXIT;
        return m_secondaryTsi;
    }


    unsigned short PersistedTrainState::getPaLibraryVersion()
    {
        FUNCTION_ENTRY("getPaLibraryVersion");
        FUNCTION_EXIT;
        return m_paLibraryVersion;
    }


    unsigned short PersistedTrainState::getTtisLibraryVersion()
    {
        FUNCTION_ENTRY("getTtisLibraryVersion");
        FUNCTION_EXIT;
        return m_ttisLibraryVersion;
    }


    unsigned short PersistedTrainState::getTtisScheduleVersion()
    {
        FUNCTION_ENTRY("getTtisScheduleVersion");
        FUNCTION_EXIT;
        return m_ttisScheduleVersion;
    }

	unsigned short PersistedTrainState::getNextTtisLibraryVersion()
	{
		FUNCTION_ENTRY("getNextTtisLibraryVersion");
		FUNCTION_EXIT;
		return m_nextttisLibraryVersion;
	}

	unsigned short PersistedTrainState::getNextTtisScheduleVersion()
    {
        FUNCTION_ENTRY("getTtisScheduleVersion");
        FUNCTION_EXIT;
        return m_nextttisScheduleVersion;
    }
	
    time_t PersistedTrainState::getTimestamp()
    {
        FUNCTION_ENTRY("getTimestamp");
        FUNCTION_EXIT;
        return m_timestamp;
    }


    void PersistedTrainState::loadData( TA_Base_Core::IData& data, unsigned int row )
    {
        FUNCTION_ENTRY("loadData");
        
        // extracts the data from each column and use DataConversion from core/utilities
        // to convert strings to numbers etc.
        // Use getColumnNames to get the column names to decode the query        
        // sets the member variable.

        // Get trainID
        m_trainId = static_cast< unsigned char >( data.getUnsignedLongData( row, TRAIN_ID_COLUMN ) );

        // Get primary_tsi
        m_primaryTsi = data.getStringData( row, PRIMARY_TSI_COLUMN );

        // Get secondary_tsi
        m_secondaryTsi = data.getStringData( row, SECONDARY_TSI_COLUMN );

        // Get pa_library_version
        m_paLibraryVersion = static_cast< unsigned short >( data.getUnsignedLongData( row, PA_LIBRARY_VERSION_COLUMN ) );

        // Get ttis_library_version
        m_ttisLibraryVersion = static_cast< unsigned short >( data.getUnsignedLongData( row, TTIS_LIBRARY_VERSION_COLUMN ) );

        // Get ttis_schedule_version
        m_ttisScheduleVersion = static_cast< unsigned short >( data.getUnsignedLongData( row, TTIS_SCHEDULE_VERSION_COLUMN ) );

		m_nextttisLibraryVersion = static_cast< unsigned short >( data.getUnsignedLongData( row, NEXT_TTIS_LIBRARY_VERSION_COLUMN ) );

		m_nextttisScheduleVersion = static_cast< unsigned short >( data.getUnsignedLongData( row, NEXT_TTIS_SCHEDULE_VERSION_COLUMN ) );

        // get Timestamp
        m_timestamp = data.getTimestampData( row, TIMESTAMP_COLUMN ).time;

        FUNCTION_EXIT;
    }

	//add by zhanghongzhi
	std::string PersistedTrainState::getLocalDatabaseName()
    {
		std::string localDatabaseName;    
		if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw TA_Base_Core::DatabaseException("db-connection-file not set");
		}
		try
		{
			TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
			localDatabaseName = connections[0].first;  
		}
		catch(...) // most likely a file not found error
		{
			throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

}
