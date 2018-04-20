/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcastHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  *
  * PaScheduleBroadcastHelper is an object that is held by PaScheduleBroadcast and ConfigPaScheduleBroadcast objects.
  * It contains all data used by PaScheduleBroadcast, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaScheduleBroadcast.
  */
#pragma warning(disable:4786 4290 4503)

#include <vector>
#include <algorithm>
#include <ctime>
#include <memory>

#include "core/data_access_interface/pa_4669/src/PaTableConsts.h"
#include "core/data_access_interface/pa_4669/src/PaScheduleBroadcastHelper.h"
#include "core/data_access_interface/pa_4669/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/ValueNotSetException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"


namespace TA_Base_Core
{

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper( const unsigned long key ) :
    m_key( key ),
    m_ulLocationKey( 0 ),
    m_ulCyclicTimes( 0 ),
    m_ulPriorityGroup( 0 ),
    m_tmStartTime( 0 ),
    m_tmEndTime( 0 ),
    m_strScheduleName( INVALID_STRING_VALUE ),
    m_strIntervalType( INVALID_STRING_VALUE ),
    m_strSourceType( INVALID_STRING_VALUE ),
    m_strStatus( INVALID_STRING_VALUE ),
    m_lstZoneKeys(),
    m_bEnable( false ),
    m_bAutoDel( false ),
    m_isValidData( false ),
    m_isNew( false ),
    m_bZonesLoaded( false ),
    m_ulScheduleId( 0 ),
    m_lstMsgKeys(),
    m_tmIntervalTime( 0 ),
    m_bSun( false ),
    m_bMon( false ),
    m_bTue( false ),
    m_bWed( false ),
    m_bThu( false ),
    m_bFri( false ),
    m_bSat( false ),
    m_bChime( false ),
    m_bMsgsLoaded( false )
{
	reload();
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper( const PaScheduleBroadcastHelper& thePaScheduleBroadcastHelper ) :
    m_key( DatabaseKey::getInvalidKey() ),
    m_ulLocationKey( thePaScheduleBroadcastHelper.m_ulLocationKey ),
    m_ulCyclicTimes( thePaScheduleBroadcastHelper.m_ulCyclicTimes ),
    m_ulPriorityGroup( thePaScheduleBroadcastHelper.m_ulPriorityGroup ),
    m_tmStartTime( thePaScheduleBroadcastHelper.m_tmStartTime ),
    m_tmEndTime( thePaScheduleBroadcastHelper.m_tmEndTime ),
    m_strScheduleName( thePaScheduleBroadcastHelper.m_strScheduleName ),
    m_strIntervalType( thePaScheduleBroadcastHelper.m_strIntervalType ),
    m_strSourceType( thePaScheduleBroadcastHelper.m_strSourceType ),
    m_strStatus( thePaScheduleBroadcastHelper.m_strStatus ),
    m_lstZoneKeys( thePaScheduleBroadcastHelper.m_lstZoneKeys ),
    m_bEnable( thePaScheduleBroadcastHelper.m_bEnable ),
    m_bAutoDel( thePaScheduleBroadcastHelper.m_bAutoDel ),
    m_isValidData( false ),
    m_isNew( true ),
    m_bZonesLoaded( false ),
    m_ulScheduleId( thePaScheduleBroadcastHelper.m_ulScheduleId ),
    m_lstMsgKeys( thePaScheduleBroadcastHelper.m_lstMsgKeys ),
    m_tmIntervalTime( thePaScheduleBroadcastHelper.m_tmIntervalTime ),
    m_bSun( thePaScheduleBroadcastHelper.m_bSun ),
    m_bMon( thePaScheduleBroadcastHelper.m_bMon ),
    m_bTue( thePaScheduleBroadcastHelper.m_bTue ),
    m_bWed( thePaScheduleBroadcastHelper.m_bWed ),
    m_bThu( thePaScheduleBroadcastHelper.m_bThu ),
    m_bFri( thePaScheduleBroadcastHelper.m_bFri ),
    m_bSat( thePaScheduleBroadcastHelper.m_bSat ),
    m_bChime( thePaScheduleBroadcastHelper.m_bChime ),
    m_bMsgsLoaded( false )
{
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper( unsigned long row, IData& data ) :
    m_key( data.getUnsignedLongData( row, KEY_COL ) ),
    m_ulLocationKey( 0 ),
    m_ulCyclicTimes( 0 ),
    m_ulPriorityGroup( 0 ),
    m_tmStartTime( 0 ),
    m_tmEndTime( 0 ),
    m_strScheduleName( INVALID_STRING_VALUE ),
    m_strIntervalType( INVALID_STRING_VALUE ),
    m_strSourceType( INVALID_STRING_VALUE ),
    m_strStatus( INVALID_STRING_VALUE ),
    m_lstZoneKeys(),
    m_bEnable( false ),
    m_bAutoDel( false ),
    m_isValidData( false ),
    m_isNew( false ),
    m_bZonesLoaded( false ),
    m_ulScheduleId( 0 ),
    m_lstMsgKeys(),
    m_tmIntervalTime( 0 ),
    m_bSun( false ),
    m_bMon( false ),
    m_bTue( false ),
    m_bWed( false ),
    m_bThu( false ),
    m_bFri( false ),
    m_bSat( false ),
    m_bChime( false ),
    m_bMsgsLoaded( false )
{
    reloadUsing( row, data );
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper() :
    m_key( DatabaseKey::getInvalidKey() ),
    m_ulLocationKey( 0 ),
    m_ulCyclicTimes( 0 ),
    m_ulPriorityGroup( 0 ),
    m_tmStartTime( 0 ),
    m_tmEndTime( 0 ),
    m_strScheduleName( INVALID_STRING_VALUE ),
    m_strIntervalType( INVALID_STRING_VALUE ),
    m_strSourceType( INVALID_STRING_VALUE ),
    m_strStatus( INVALID_STRING_VALUE ),
    m_lstZoneKeys(),
    m_bEnable( false ),
    m_bAutoDel( false ),
    m_isValidData( false ),
    m_isNew( true ),
    m_bZonesLoaded( false ),
    m_ulScheduleId( 0 ),
    m_lstMsgKeys(),
    m_tmIntervalTime( 0 ),
    m_bSun( false ),
    m_bMon( false ),
    m_bTue( false ),
    m_bWed( false ),
    m_bThu( false ),
    m_bFri( false ),
    m_bSat( false ),
    m_bChime( false ),
    m_bMsgsLoaded( false )
{
}


PaScheduleBroadcastHelper::~PaScheduleBroadcastHelper()
{
}


unsigned long PaScheduleBroadcastHelper::getKey()
{
    FUNCTION_ENTRY( "getKey" );

    if( !m_isValidData && !m_isNew )
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_key;
}

unsigned long PaScheduleBroadcastHelper::getLocationKey()
{
    FUNCTION_ENTRY( "getLocationKey" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    FUNCTION_EXIT;

    return m_ulLocationKey;

}

void PaScheduleBroadcastHelper::setLocationKey( const unsigned long locationKey )
{
    FUNCTION_ENTRY( "setLocationKey" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_ulLocationKey = locationKey;

    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getDvaMsgKey()
{
    FUNCTION_ENTRY( "getDvaMsgKey" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }

    // Following the reload of data, all values should have been set
    if( isNew() && m_lstMsgKeys.empty() )
    {
        return DatabaseKey::getInvalidKey();
    }

    FUNCTION_EXIT;
    return m_lstMsgKeys.front();

}

void PaScheduleBroadcastHelper::setDvaMsgKey( unsigned long ulDvaMsgKey )
{
    FUNCTION_ENTRY( "setDvaMsgKey" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_lstMsgKeys.clear();
    m_lstMsgKeys.push_back( ulDvaMsgKey );

    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getCyclicTimes()
{
    FUNCTION_ENTRY( "getCyclicTimes" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }

    FUNCTION_EXIT;
    return m_ulCyclicTimes;
}

void PaScheduleBroadcastHelper::setCyclicTimes( unsigned long ulCyclicTimes )
{
    FUNCTION_ENTRY( "setCyclicTimes" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_ulCyclicTimes = ulCyclicTimes;

    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getPriorityGroup()
{
    FUNCTION_ENTRY( "getPriorityGroup" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }

    FUNCTION_EXIT;
    return m_ulPriorityGroup;
}

void PaScheduleBroadcastHelper::setPriorityGroup( unsigned long ulPriorityGroup )
{
    FUNCTION_ENTRY( "setPriorityGroup" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_ulPriorityGroup = ulPriorityGroup;

    FUNCTION_EXIT;
}

time_t PaScheduleBroadcastHelper::getStartTime()
{
    FUNCTION_ENTRY( "getStartTime" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_tmStartTime;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setStartTime( time_t tmStartTime )
{
    FUNCTION_ENTRY( "setStartTime" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_tmStartTime = tmStartTime;

    FUNCTION_EXIT;
}

time_t PaScheduleBroadcastHelper::getEndTime()
{
    FUNCTION_ENTRY( "getEndTime" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_tmEndTime;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setEndTime( time_t tmEndTime )
{
    FUNCTION_ENTRY( "setEndTime" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_tmEndTime = tmEndTime;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getScheduleName()
{
    FUNCTION_ENTRY( "getScheduleName" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_strScheduleName;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setScheduleName( const std::string& strScheduleName )
{
    FUNCTION_ENTRY( "setScheduleName" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_strScheduleName = strScheduleName;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getIntervalType()
{
    FUNCTION_ENTRY( "getIntervalType" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_strIntervalType;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setIntervalType( const std::string& strIntervalType )
{
    FUNCTION_ENTRY( "setIntervalType" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_strIntervalType = strIntervalType;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getSourceType()
{
    FUNCTION_ENTRY( "getSourceType" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_strSourceType;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setSourceType( const std::string& strSourceType )
{
    FUNCTION_ENTRY( "setSourceType" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_strSourceType = strSourceType;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getStatus()
{
    FUNCTION_ENTRY( "getStatus" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_strStatus;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setStatus( const std::string& strStatus )
{
    FUNCTION_ENTRY( "setStatus" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_strStatus = strStatus;

    FUNCTION_EXIT;
}

std::list<unsigned long> PaScheduleBroadcastHelper::getZoneKeys()
{
    FUNCTION_ENTRY( "getSourceType" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    else if( !m_bZonesLoaded )
    {
        reloadZones();
    }

    return m_lstZoneKeys;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setZoneKeys( const std::list<unsigned long>& lstZoneKeys )
{
    FUNCTION_ENTRY( "setZoneKeys" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_lstZoneKeys = lstZoneKeys;

    FUNCTION_EXIT;
}

bool PaScheduleBroadcastHelper::getIsEnable()
{
    FUNCTION_ENTRY( "getIsEnable" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bEnable;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setIsEnable( bool bEnable )
{
    FUNCTION_ENTRY( "setIsEnable" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bEnable = bEnable;

    FUNCTION_EXIT;
}

bool PaScheduleBroadcastHelper::getAutoDel()
{
    FUNCTION_ENTRY( "getAutoDel" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bAutoDel;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setAutoDel( bool bAutoDel )
{
    FUNCTION_ENTRY( "setAutoDel" );

    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bAutoDel = bAutoDel;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::invalidate()
{
    FUNCTION_ENTRY( "invalidate" );

    // This method cannot be called until the key has been set
    TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new PaDvaMessage" );

    m_isValidData = false;

    FUNCTION_EXIT;
}

const DatabaseQueryHelper::QueryData PaScheduleBroadcastHelper::getBasicQueryData()
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

    DatabaseQueryHelper::createBasicSelectStatement( result.sqlStatement, result.columnNames, result.tableName, false );

    return result;
}


void PaScheduleBroadcastHelper::reloadUsing( unsigned long row, IData& data )
{
    FUNCTION_ENTRY( "reloadUsing" );

    // Assign the data as appropriate to the member variables.
    // These calls can throw DataExceptions, as documented in the comment of this method.
    m_ulLocationKey = data.getUnsignedLongData( row, LOCATION_KEY_COL );
    m_strScheduleName = data.getStringData( row, PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL );
    m_tmStartTime = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_START_TIME_COL );
    m_tmEndTime = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_END_TIME_COL );
    m_strIntervalType = data.getStringData( row, PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL );
    m_bEnable = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_IS_ENABLE_COL );
    m_bAutoDel = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_AUTO_DEL_COL );
    m_ulCyclicTimes = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL );
    m_ulPriorityGroup = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL );
    m_strSourceType = data.getStringData( row, PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL );
    m_strStatus = data.getStringData( row, PA_SCHEDULE_BROADCAST_STATUS_COL );

    m_ulScheduleId = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_SCHEDULE_ID_COL );
    m_tmIntervalTime = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_INTERVAL_TIME_COL );
    m_bChime = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_CHIME_COL );
    m_bSun = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_SUN_COL );
    m_bMon = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_MON_COL );
    m_bTue = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_TUE_COL );
    m_bWed = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_WED_COL );
    m_bThu = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_THU_COL );
    m_bFri = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_FRI_COL );
    m_bSat = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_SAT_COL );

    // Need to record that we now have valid data
    m_isValidData = true;
    FUNCTION_EXIT;
}


void PaScheduleBroadcastHelper::writePaScheduleBroadcast() //throw(TransactiveException)
{
    FUNCTION_ENTRY( "writePaScheduleBroadcast" );

    // Check ALL values exist. If not add them to the dataConfiguratonException

    std::vector<std::string> fieldNames;
    if( m_lstMsgKeys.empty() )
    {
        fieldNames.push_back( PA_SCHEDULE_BROADCAST_MSGS_MSG_KEY_COL );
    }

    if( m_lstZoneKeys.empty() )
    {
        fieldNames.push_back( PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL );
    }

    if( !fieldNames.empty() )
    {
        TA_THROW( DataConfigurationException(
                      "PaScheduleBroadcast data not fully specified. PaScheduleBroadcast cannot be written to database",
                      fieldNames ) );
    }

    try
    {
        // Need to check if this is a new PaScheduleBroadcast or an existing PaScheduleBroadcast -
        // as a new PaScheduleBroadcast will be inserted, while an existing PaScheduleBroadcast will be updated
        if( m_isNew ) // This is a new PaScheduleBroadcast
        {
            addNewPaScheduleBroadcast();
        }
        else // This is an existing PaScheduleBroadcast
        {
            modifyExistingPaScheduleBroadcast();
        }
    }
    catch( ValueNotSetException& )
    {
        // Should not get here if properly validated set state of fields before calling
        // statements in the try block
        std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

        TA_THROW( DataConfigurationException(
                      "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                      dummyNames ) );
    }

    // Now that the data has been written, what we hold is valid data.
    m_isValidData = true;
    m_isNew = false;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::deletePaScheduleBroadcast()
{
    FUNCTION_ENTRY( "deletePaScheduleBroadcast" );

    TA_ASSERT( !m_isNew, "This PaScheduleBroadcast does not yet exist in the database, and therefore cannot be deleted" );

    deleteZonesConfig();

    deleteMsgsConfig();

    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );
    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_DELETE_85003, getLocalDatabaseName(), m_ulLocationKey, m_ulScheduleId );
    databaseConnection->executeModification( strSql );

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::modifyExistingPaScheduleBroadcast() //throw(TransactiveException)
{
    FUNCTION_ENTRY( "modifyExistingPaScheduleBroadcast" );

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
         "PaScheduleBroadcast %lu already exists. It's data will be updated.", m_key );

    validateLocationKeyExistence();

    ////////////////////////////////////////////////////////////////////////////
    //
    // Good to go...
    //
    ////////////////////////////////////////////////////////////////////////////
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );
    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_UPDATE_85001, getLocalDatabaseName(),
                                             m_ulLocationKey,
                                             databaseConnection->escapeAQSQLString( m_strScheduleName ),
                                             m_tmStartTime,
                                             m_tmEndTime,
                                             databaseConnection->escapeAQSQLString( m_strIntervalType ),
                                             m_bEnable,
                                             m_bAutoDel,
                                             m_ulCyclicTimes,
                                             m_ulPriorityGroup,
                                             databaseConnection->escapeAQSQLString( m_strSourceType ),
                                             databaseConnection->escapeAQSQLString( m_strStatus ),
                                             m_ulScheduleId,
                                             m_tmIntervalTime,
                                             m_bChime,
                                             m_bSun,
                                             m_bMon,
                                             m_bTue,
                                             m_bWed,
                                             m_bThu,
                                             m_bFri,
                                             m_bSat,
                                             m_ulLocationKey,
											 m_ulScheduleId);

    databaseConnection->executeModification( strSql );

    deleteZonesConfig();
    insertZonesConfig();

    deleteMsgsConfig();
    insertMsgsConfig();

    // nothing more to be done here
    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::addNewPaScheduleBroadcast() //throw(TransactiveException)
{
    FUNCTION_ENTRY( "addNewPaScheduleBroadcast" );

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
         "PaScheduleBroadcast %u is a new one. It will be added to the database.", m_ulLocationKey );

    validateLocationKeyExistence();


    ////////////////////////////////////////////////////////////////////////////
    //
    // Good to go...
    //
    ////////////////////////////////////////////////////////////////////////////
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );

    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_INSERT_85002, getLocalDatabaseName(),
                                             m_ulLocationKey,
                                             databaseConnection->escapeAQSQLString( m_strScheduleName ),
                                             m_tmStartTime,
                                             m_tmEndTime,
                                             databaseConnection->escapeAQSQLString( m_strIntervalType ),
                                             m_bEnable,
                                             m_bAutoDel,
                                             m_ulCyclicTimes,
                                             m_ulPriorityGroup,
                                             databaseConnection->escapeAQSQLString( m_strSourceType ),
                                             databaseConnection->escapeAQSQLString( m_strStatus ),
                                             m_ulScheduleId,
                                             m_tmIntervalTime,
                                             m_bChime,
                                             m_bSun,
                                             m_bMon,
                                             m_bTue,
                                             m_bWed,
                                             m_bThu,
                                             m_bFri,
                                             m_bSat );
    databaseConnection->executeModification( strSql );


    std::vector<std::string> columnNames;
    columnNames.push_back( KEY_COL );

    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_SELECT_85004, m_ulScheduleId, m_ulLocationKey );
    std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

    if( 0 == data->getNumRows() )
    {
        std::ostringstream reasonMessage;
        reasonMessage << "No data found for locationKey = " << m_ulLocationKey << " scheduleId = " << m_ulScheduleId;
        TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE, "PKEY" ) );
    }

    // Retrieve the pkey as an unsigned long. There should only be a single returned item
    // (as any other circumstance is specifically prevented by the above checks).
    // This method can throw a DataException
    m_key = data->getUnsignedLongData( 0, KEY_COL );

    insertZonesConfig();
    insertMsgsConfig();

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::validateLocationKeyExistence() //throw(TransactiveException)
{
    /*
    std::ostringstream sql;
    sql << "select PKEY from LOCATION where PKEY = " << m_ulLocationKey;

    std::vector<std::string> columnNames;
    columnNames.clear();
    columnNames.push_back( "PKEY" );

    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );
    IData* data = databaseConnection->executeQuery( sql.str(), columnNames );

    if( 0 == data->getNumRows() )
    {
        // Location key not found
        delete data;
        data = NULL;

        std::ostringstream reasonMessage;
        reasonMessage << "No data found for Location key = " << m_ulLocationKey;
        TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE, "Location key" ) );
    }

    delete data;
    data = NULL;
    */
}

void PaScheduleBroadcastHelper::reloadAll()
{
    reload();
    reloadZones();
    reloadMsgs();
}

void PaScheduleBroadcastHelper::reloadZones()
{
    m_lstZoneKeys.clear();

    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

    // Set up the columnNames vector to be passed to executeQuery()
    std::vector<std::string> columnNames;
    columnNames.push_back( PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL );

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_ZONES_SELECT_87001, m_ulLocationKey, m_ulScheduleId );
    IData* nakeDataPtr = databaseConnection->executeQuery( strSql, columnNames );
    do
    {
        std::auto_ptr<IData> data( nakeDataPtr );
        nakeDataPtr = NULL;

        if( 0 == data->getNumRows() )
        {
            break;
        }

        for( unsigned long i = 0, sz = data->getNumRows(); i < sz; ++i )
        {
            m_lstZoneKeys.push_back( data->getUnsignedLongData( i, PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL ) );
        }
    }
    while( databaseConnection->moreData( nakeDataPtr ) );

    m_bZonesLoaded = true;
}

std::list<unsigned long> PaScheduleBroadcastHelper::getDvaMsgKeys()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    else if( !m_bMsgsLoaded )
    {
        reloadMsgs();
    }

    return m_lstMsgKeys;
}

time_t PaScheduleBroadcastHelper::getIntervalTime()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_tmIntervalTime;
}

bool PaScheduleBroadcastHelper::getIsSun()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bSun;
}

bool PaScheduleBroadcastHelper::getIsMon()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bMon;
}

bool PaScheduleBroadcastHelper::getIsTue()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bTue;
}

bool PaScheduleBroadcastHelper::getIsWed()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bWed;
}

bool PaScheduleBroadcastHelper::getIsThu()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bThu;
}

bool PaScheduleBroadcastHelper::getIsFri()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bFri;
}

bool PaScheduleBroadcastHelper::getIsSat()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bSat;
}

bool PaScheduleBroadcastHelper::getIsChime()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_bChime;
}

void PaScheduleBroadcastHelper::setDvaMsgKeys( const std::list<unsigned long>& lstMsgKeys )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_lstMsgKeys = lstMsgKeys;
}

void PaScheduleBroadcastHelper::setIntervalTime( time_t tmIntervalTime )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_tmIntervalTime = tmIntervalTime;
}

void PaScheduleBroadcastHelper::setIsSun( bool bSun )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bSun = bSun;
}

void PaScheduleBroadcastHelper::setIsMon( bool bMon )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bMon = bMon;
}

void PaScheduleBroadcastHelper::setIsTue( bool bTue )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bTue = bTue;
}

void PaScheduleBroadcastHelper::setIsWed( bool bWed )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bWed = bWed;
}

void PaScheduleBroadcastHelper::setIsThu( bool bThu )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bThu = bThu;
}

void PaScheduleBroadcastHelper::setIsFri( bool bFri )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bFri = bFri;
}

void PaScheduleBroadcastHelper::setIsSat( bool bSat )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bSat = bSat;
}

void PaScheduleBroadcastHelper::setIsChime( bool bChime )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_bChime = bChime;
}

unsigned long PaScheduleBroadcastHelper::getScheduleId()
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    return m_ulScheduleId;
}

void PaScheduleBroadcastHelper::setScheduleId( unsigned long scheduleId )
{
    if( !m_isValidData && !m_isNew )
    {
        reloadAll();
    }
    m_ulScheduleId = scheduleId;
}

void PaScheduleBroadcastHelper::reloadMsgs()
{
    m_lstMsgKeys.clear();
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

    // Set up the columnNames vector to be passed to executeQuery()
    std::vector<std::string> columnNames;
    columnNames.push_back( PA_SCHEDULE_BROADCAST_MSGS_MSG_KEY_COL );

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_MSGS_SELECT_88001, m_ulLocationKey, m_ulScheduleId );

    IData* nakeDataPtr = databaseConnection->executeQuery( strSql, columnNames );

    do
    {
        std::auto_ptr<IData> data( nakeDataPtr );
        nakeDataPtr = NULL;

        if( 0 == data->getNumRows() )
        {
            break;
        }

        for( unsigned long i = 0, sz = data->getNumRows(); i < sz; ++i )
        {
            m_lstMsgKeys.push_back( data->getUnsignedLongData( i, PA_SCHEDULE_BROADCAST_MSGS_MSG_KEY_COL ) );
        }
    }
    while( databaseConnection->moreData( nakeDataPtr ) );

    m_bMsgsLoaded = true;
}

void PaScheduleBroadcastHelper::deleteZonesConfig()
{
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );

    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_ZONES_DELETE_87002, getLocalDatabaseName(), m_ulScheduleId, m_ulLocationKey );
    databaseConnection->executeModification( strSql );
}

void PaScheduleBroadcastHelper::insertZonesConfig()
{
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );

    for( std::list<unsigned long>::iterator itLoop = m_lstZoneKeys.begin(); m_lstZoneKeys.end() != itLoop; ++itLoop )
    {
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_ZONES_INSERT_87003, getLocalDatabaseName(), m_ulScheduleId, m_ulLocationKey, *itLoop );
        databaseConnection->executeModification( strSql );
    }

}

void PaScheduleBroadcastHelper::deleteMsgsConfig()
{
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );

    SQLStatement strSql;
    databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_MSGS_DELETE_88002, getLocalDatabaseName(), m_ulScheduleId, m_ulLocationKey );
    databaseConnection->executeModification( strSql );
}

void PaScheduleBroadcastHelper::insertMsgsConfig()
{
    // get a connection to the database
    IDatabase* databaseConnection =
        DatabaseFactory::getInstance().getDatabase( Pa_Cd, Write );

    for( std::list<unsigned long>::iterator itLoop = m_lstMsgKeys.begin(); m_lstMsgKeys.end() != itLoop; ++itLoop )
    {
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement( strSql, PA_SCHEDULE_BROADCAST_MSGS_INSERT_88003, getLocalDatabaseName(), m_ulScheduleId, m_ulLocationKey, *itLoop );
        databaseConnection->executeModification( strSql );
    }
}

std::string PaScheduleBroadcastHelper::getLocalDatabaseName()
{
    if( m_localDatabase.empty() )
    {

        // have to find out what is the local db name
        // this should be the first in the connection list so grab it
        if( ! RunParams::getInstance().isSet( RPARAM_DBCONNECTIONFILE ) )
        {
            throw DatabaseException( "db-connection-file not set" );
        }

        try
        {
            DbConnectionStrings connectionStrings( RunParams::getInstance().get( RPARAM_DBCONNECTIONFILE ) );
            DataConnections connections = connectionStrings.getConnectionList( Sessions_Ad, Write );
            m_localDatabase = connections[0].first;
        }
        //exception
        catch( ... ) // most likely a file not found error
        {
            throw DatabaseException( "Unable to find database connection infomation" );
        }

    }
    return m_localDatabase;
}

} // closes TA_IRS_Core