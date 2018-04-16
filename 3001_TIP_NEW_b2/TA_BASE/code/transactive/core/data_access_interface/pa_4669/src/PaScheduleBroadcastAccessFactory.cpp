/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  *
  * PaScheduleBroadcastAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)

#include "core/data_access_interface/pa_4669/src/PaTableConsts.h"
#include "core/data_access_interface/pa_4669/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa_4669/src/PaScheduleBroadcast.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaScheduleBroadcast.h"
#include "core/data_access_interface/pa_4669/src/PaScheduleBroadcastAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include <memory>


namespace TA_Base_Core
{
PaScheduleBroadcastAccessFactory* PaScheduleBroadcastAccessFactory::m_instance = 0;

PaScheduleBroadcastAccessFactory& PaScheduleBroadcastAccessFactory::getInstance()
{
    if( m_instance == 0 )
    {
        m_instance = new PaScheduleBroadcastAccessFactory();
    }
    return *m_instance;
}

void PaScheduleBroadcastAccessFactory::removeInstance( )
{
    if( m_instance != 0 )
    {
        delete m_instance;
        m_instance = 0;
    }
}

IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getAllPaScheduleBroadcasts( const bool readWrite )
{
    return getPaScheduleBroadcastsReturnedFromStatement( "", readWrite );
}

IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getPaScheduleBroadcastsByLocationKey( unsigned long locationKey, bool bRequireEnable, bool readWrite )
{
    std::ostringstream sql;
    sql << LOCATION_KEY_COL << " = " << locationKey;
    if( bRequireEnable )
    {
        sql << " and " << PA_SCHEDULE_BROADCAST_IS_ENABLE_COL << " = 1";
    }

    return getPaScheduleBroadcastsReturnedFromStatement( sql.str(), readWrite );
}

IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getPaScheduleBroadcastsReturnedFromStatement( const std::string& whereClause,  const bool readWrite )
{
    DatabaseQueryHelper::QueryData result;

    result.primaryKeyCol = KEY_COL;
    result.tableName = PA_SCHEDULE_BROADCAST_TABLE;

    result.columnNames.push_back( KEY_COL );
    result.columnNames.push_back( LOCATION_KEY_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_START_TIME_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_END_TIME_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_IS_ENABLE_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_AUTO_DEL_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_STATUS_COL );

    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_SCHEDULE_ID_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_INTERVAL_TIME_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_CHIME_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_SUN_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_MON_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_TUE_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_WED_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_THU_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_FRI_COL );
    result.columnNames.push_back( PA_SCHEDULE_BROADCAST_SAT_COL );

    DatabaseQueryHelper::createBasicSelectStatement( result.sqlStatement, result.columnNames,
                                                     result.tableName, false );

    return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
           <IPaScheduleBroadcasts, ConfigPaScheduleBroadcast, PaScheduleBroadcast>
           ( result, whereClause, readWrite );
}

IConfigPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::createPaScheduleBroadcast()
{
    return new ConfigPaScheduleBroadcast();
}

IConfigPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::copyPaScheduleBroadcast( const IConfigPaScheduleBroadcast* PaScheduleBroadcastToCopy )
{
    FUNCTION_ENTRY( "copyPaScheduleBroadcast" );

    TA_ASSERT( PaScheduleBroadcastToCopy !=  NULL, "The PaScheduleBroadcast to copy was NULL" );

    const ConfigPaScheduleBroadcast* cast = dynamic_cast<const ConfigPaScheduleBroadcast*>( PaScheduleBroadcastToCopy );

    TA_ASSERT( cast != NULL, "PaScheduleBroadcast passed could not be converted into a ConfigPaScheduleBroadcast" );

    FUNCTION_EXIT;
    return new ConfigPaScheduleBroadcast( *cast );

}

IPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::getPaScheduleBroadcastByScheduleIdAndLocationKey( const unsigned long scheduleId, const unsigned long locationKey, const bool readWrite )
{
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

    std::vector<std::string> columnNames;
    columnNames.push_back( KEY_COL );

    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_SELECT_85004, scheduleId, locationKey );
	std::auto_ptr<IData> data(databaseConnection->executeQuery( strSql, columnNames ));

    if( 0 == data->getNumRows() ) // No entry found with the specified pkey
    {
        char reasonMessage[256] = {0};
        sprintf( reasonMessage, "No data found for scheduleId = %lu", scheduleId );
        TA_THROW( DataException( reasonMessage, DataException::NO_VALUE, "" ) );
    }
	/*
    else if( 1 < data->getNumRows() ) // More than one entry found for the pkey
    {
        char reasonMessage[256];
        sprintf( reasonMessage, "More than one entry found for scheduleId = %lu", scheduleId );
        TA_THROW( DataException( reasonMessage, DataException::NOT_UNIQUE, "" ) );
    }
	*/

	unsigned long key = data->getUnsignedLongData(0, KEY_COL);
    // Create the PaDvaMessage object to represent this object. Initially only populate it
    // with id. We will populate the rest of the data on demand.
    IPaScheduleBroadcast* thePaScheduleBroadcast;

    // decide which PaDvaMessage object to return
    if( readWrite ) // need a config PaDvaMessage
    {
        thePaScheduleBroadcast = new ConfigPaScheduleBroadcast( key );
    }
    else // need a standard PaDvaMessage
    {
        thePaScheduleBroadcast = new PaScheduleBroadcast( key );
    }

    // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
    // for deleting it.
    return thePaScheduleBroadcast;
}

} // closes TA_IRS_Core


