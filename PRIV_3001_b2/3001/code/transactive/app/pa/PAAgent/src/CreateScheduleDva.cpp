/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/CreateScheduleDva.cpp $
  * @author:  HuangQi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/CreateScheduleDva.h"


namespace TA_IRS_App
{

CreateScheduleDva::CreateScheduleDva( const bool bAtOcc, ITransactionOwner* pOwner ) : 
PasTransaction( bAtOcc, pOwner )
{
}

CreateScheduleDva::~CreateScheduleDva()
{
}

void CreateScheduleDva::setWithChime( const bool bWithChime )
{
    m_bWithChime = bWithChime;
}

void CreateScheduleDva::setDaysInWeek( const unsigned short usDaysInWeek )
{
    m_usDaysInWeek = usDaysInWeek;
}

void CreateScheduleDva::setStartTime( const unsigned short usStartTime, const unsigned short usStartSecond )
{
    m_usStartTime = usStartTime;
    m_usStartSecond = usStartSecond;
}

void CreateScheduleDva::setEndTime( const unsigned short usEndTime, const unsigned short usEndSecond )
{
    m_usEndTime = usEndTime;
    m_usEndSecond = usEndSecond;
}

void CreateScheduleDva::setInterval( const unsigned short usInterval, const unsigned short usIntervalSecond  )
{
    m_usInterval = usInterval;
    m_usIntervalSecond = usIntervalSecond;
}

void CreateScheduleDva::setScheduleId( const unsigned short usScheduleId )
{
    // No limitation, when out of range, PAS return Not OK.
    m_usScheduleId = usScheduleId;
}

void CreateScheduleDva::setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds )
{
    m_vecDvaMessageIds = vecMsgIds;
}

void CreateScheduleDva::setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys )
{
    m_vecPasZoneKeys = vecPasZoneKeys;
}

void CreateScheduleDva::setDwellPeriod( const unsigned short usDwellPeriod )
{
	m_usDwellPeriod = usDwellPeriod;
}

int CreateScheduleDva::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute CreateScheduleDva" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set message ID
	if( 1u == m_vecDvaMessageIds.size() )
	{
        dbTempPasData.setRange( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_ADDRESS_OCC_MESSAGE_ID );
		std::vector<unsigned short>::iterator itFirst = m_vecDvaMessageIds.begin();
        dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, *itFirst );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
            return TRANSACTION_CONNECTION_FAILURE;
        }
	}
	else
	{
		//multi-message broadcast
		dbTempPasData.setRange( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_ADDRESS_OCC_MESSAGE_ID );
		dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, (~m_vecDvaMessageIds.size() + 1 ) );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set messages count" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_MULTI_MESSAGES_ID_START, PAS_ADDRESS_MULTI_MESSAGES_ID_END );
        PasHelpers::getInstance()->formatMultiMessagesIds(m_vecDvaMessageIds, dbTempPasData);
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Multi-Messages ID" );
            return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_REPEAT_TIME, PAS_ADDRESS_OCC_SCHEDULE_REPEAT_TIME );
		dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_REPEAT_TIME, PAS_SCHEDULE_DEFAULT_REPEAT_TIME );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set repeat time" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
    }

    // 2. Set schedule event ID
    dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_ID, PAS_ADDRESS_OCC_SCHEDULE_ID );
    dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_ID, m_usScheduleId );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set schedule event ID" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Set matrix routing table
    dbTempPasData.setAll( PAS_ZONE_STATE_FREE );
    dbTempPasData.setRange( PAS_ADDRESS_OCC_MATRIX_TABLE_START, PAS_ADDRESS_OCC_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasOccZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Set priority, always 0, controlled by Pas
    dbTempPasData.setRange( PAS_ADDRESS_DVA_PRIORITY, PAS_ADDRESS_DVA_PRIORITY );
    dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0 );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Set scheduler start time
    dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_START_TIME, PAS_ADDRESS_OCC_SCHEDULE_START_TIME );
    dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_START_TIME, m_usStartTime );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Set scheduler end time
    dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_END_TIME, PAS_ADDRESS_OCC_SCHEDULE_END_TIME );
    dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_END_TIME, m_usEndTime );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler end time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 7. Set scheduler period
    dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_PERIOD, PAS_ADDRESS_OCC_SCHEDULE_PERIOD );
    dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_PERIOD, m_usInterval );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler period" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

	// 8. Set dwell period
	if( 1u < m_vecDvaMessageIds.size() )
	{
		dbTempPasData.setRange( PAS_ADDRESS_OCC_DWELL_PERIOD, PAS_ADDRESS_OCC_DWELL_PERIOD );
		dbTempPasData.set( PAS_ADDRESS_OCC_DWELL_PERIOD, m_usDwellPeriod );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set dwell period" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
	}

    // 9. Set scheduler day
    dbTempPasData.setRange( PAS_ADDRESS_OCC_SCHEDULE_DAYS, PAS_ADDRESS_OCC_SCHEDULE_DAYS );
    dbTempPasData.set( PAS_ADDRESS_OCC_SCHEDULE_DAYS, m_usDaysInWeek );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler day" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 10. Reset execution register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 11. Write command to create scheduled audio message
    const unsigned short usCommandCreateSchedule = 
        m_bWithChime ? PAS_COMMAND_OCC_CREATE_CHIME_SCHEDULE : PAS_COMMAND_OCC_CREATE_SCHEDULE;
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandCreateSchedule );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to create scheduled audio message" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 12. Set execution register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 13. read execution register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 14. Read command acknowledge register
    unsigned short usResult = 0;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CreateScheduleDva execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandCreateSchedule == usResult )
    {
        return TRANSACTION_FAILED;
    }
    else
    {
        // Not defined yet
        return TRANSACTION_FAILED;
    }

    return TRANSACTION_SUCCESS;
}

int CreateScheduleDva::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute CreateScheduleDva" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

	// 1. Set Type of Broadcast
	dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_DVA_SOURCE_TYPE, PAS_ADDRESS_STN_SCHEDULE_DVA_SOURCE_TYPE );
	dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_DVA_SOURCE_TYPE, PAS_STN_SCHEDULE_DVA_SOURCE_TYPE );
	if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set schedule source type" );
		return TRANSACTION_CONNECTION_FAILURE;
	}

    // 2. Set message ID ( address is source id, confirmed, if changed to another new address, should add a new constant )
	if( 1u == m_vecDvaMessageIds.size() )
	{
		dbTempPasData.setRange( PAS_ADDRESS_STN_MESSAGE_ID, PAS_ADDRESS_STN_MESSAGE_ID );
		std::vector<unsigned short>::iterator itFirst = m_vecDvaMessageIds.begin();
		dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, PAS_STN_SCHEDULE_MSG_ID_BASE + *itFirst );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
	}
	else
	{
		//multi-message broadcast
		dbTempPasData.setRange( PAS_ADDRESS_STN_MESSAGE_ID, PAS_ADDRESS_STN_MESSAGE_ID );
		dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, (~m_vecDvaMessageIds.size() + 1 ) );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set messages count" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_MULTI_MESSAGES_ID_START, PAS_ADDRESS_MULTI_MESSAGES_ID_END );
		PasHelpers::getInstance()->formatMultiMessagesIds(m_vecDvaMessageIds, dbTempPasData);
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Multi-Messages ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_REPEAT_TIME, PAS_ADDRESS_STN_SCHEDULE_REPEAT_TIME );
		dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_REPEAT_TIME, PAS_SCHEDULE_DEFAULT_REPEAT_TIME );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set repeat time" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
    }

    // 3. Set schedule event ID
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_ID, PAS_ADDRESS_STN_SCHEDULE_ID );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_ID, m_usScheduleId );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set schedule event ID" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Set matrix routing table
    dbTempPasData.setAll( PAS_ZONE_STATE_FREE );
    dbTempPasData.setRange( PAS_ADDRESS_STN_MATRIX_TABLE_START, PAS_ADDRESS_STN_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasStnZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Set priority, always 0, controlled by Pas
    dbTempPasData.setRange( PAS_ADDRESS_DVA_PRIORITY, PAS_ADDRESS_DVA_PRIORITY );
    dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Set scheduler start year
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_START_YEAR, PAS_ADDRESS_STN_SCHEDULE_START_YEAR );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_START_YEAR, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 7. Set scheduler start day
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_START_DAY, PAS_ADDRESS_STN_SCHEDULE_START_DAY );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_START_DAY, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 8. Set scheduler start time
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_START_TIME, PAS_ADDRESS_STN_SCHEDULE_START_TIME );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_START_TIME, m_usStartTime );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 9. Set scheduler stop year
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_STOP_YEAR, PAS_ADDRESS_STN_SCHEDULE_STOP_YEAR );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_STOP_YEAR, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 10. Set scheduler stop day
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_STOP_DAY, PAS_ADDRESS_STN_SCHEDULE_STOP_DAY );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_STOP_DAY, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler start time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 11. Set scheduler day
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_DAYS, PAS_ADDRESS_STN_SCHEDULE_DAYS );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_DAYS, m_usDaysInWeek );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler day" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 12. Set scheduler end time
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_END_TIME, PAS_ADDRESS_STN_SCHEDULE_END_TIME );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_END_TIME, m_usEndTime );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler end time" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 13. Set scheduler period
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_PERIOD, PAS_ADDRESS_STN_SCHEDULE_PERIOD );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_PERIOD, m_usInterval );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set scheduler period" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

	// 14. Set dwell period
	if( 1u < m_vecDvaMessageIds.size() )
	{
		dbTempPasData.setRange( PAS_ADDRESS_STN_DWELL_PERIOD, PAS_ADDRESS_STN_DWELL_PERIOD );
		dbTempPasData.set( PAS_ADDRESS_STN_DWELL_PERIOD, m_usDwellPeriod );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set dwell period" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
	}

    // 15. Reset execution register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 16. Write command to create scheduled audio message
    const unsigned short usCommandCreateSchedule = 
        m_bWithChime ? PAS_COMMAND_STN_CREATE_CHIME_SCHEDULE : PAS_COMMAND_STN_CREATE_SCHEDULE;
    dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, usCommandCreateSchedule );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to create scheduled audio message" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 17. Set execution register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 18. Read execution register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 19. Read command acknowledge register
    unsigned short usResult = 0;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CreateScheduleDva execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandCreateSchedule == usResult )
    {
        return TRANSACTION_FAILED;
    }
    else
    {
        // Not defined yet
        return TRANSACTION_FAILED;
    }

    return TRANSACTION_SUCCESS;
}

}
