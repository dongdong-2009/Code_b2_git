/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StartDvaBroadcast.cpp $
  * @author:  HuangQi
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/StartDvaBroadcast.h"


namespace TA_IRS_App
{

StartDvaBroadcast::StartDvaBroadcast( const bool bAtOcc, ITransactionOwner* pOwner ) : 
PasTransaction( bAtOcc, pOwner )
{
}

StartDvaBroadcast::~StartDvaBroadcast()
{
}

int StartDvaBroadcast::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaBroadcast" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set matrix routing table
    dbTempPasData.setRange( PAS_ADDRESS_OCC_MATRIX_TABLE_START, PAS_ADDRESS_OCC_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasOccZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Set priority, always 0, controlled by Pas
    dbTempPasData.setRange( PAS_ADDRESS_DVA_PRIORITY, PAS_ADDRESS_DVA_PRIORITY );
    dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

	// 3.Set transaction id
	dbTempPasData.setRange( PAS_ADDRESS_OCC_TRANSACTION_ID, PAS_ADDRESS_OCC_TRANSACTION_ID );
	dbTempPasData.set( PAS_ADDRESS_OCC_TRANSACTION_ID, m_usTransationId );
	if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set transaction id" );
		return TRANSACTION_CONNECTION_FAILURE;
	}

    // 4. Set message ID

	//modified by lixiaoxia
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
		dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, (~m_vecDvaMessageIds.size() + 1u ));
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set messages count" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_MULTI_MESSAGES_ID_START, PAS_ADDRESS_MULTI_MESSAGES_ID_END );
		PasHelpers::getInstance()->formatMultiMessagesIds( m_vecDvaMessageIds, dbTempPasData );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Multi-Messages ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		dbTempPasData.setRange( PAS_ADDRESS_OCC_DWELL_PERIOD, PAS_ADDRESS_OCC_DWELL_PERIOD );
		dbTempPasData.set( PAS_ADDRESS_OCC_DWELL_PERIOD, m_usDwellPeriod );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set dwell period" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
	}

    // 5. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Write command to start DVA announcement
    const unsigned short usCommandStartDva = 
        m_bWithChime ? PAS_COMMAND_OCC_START_CHIME_DVA : PAS_COMMAND_OCC_START_DVA;
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandStartDva );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 7. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 8. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 9. Read Command Acknowledge Register
    unsigned short usResult = 0u;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaBroadcast execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandStartDva == usResult )
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

int StartDvaBroadcast::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaBroadcast" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set Matrix Routing Table
    dbTempPasData.setRange( PAS_ADDRESS_STN_MATRIX_TABLE_START, PAS_ADDRESS_STN_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasStnZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Set priority, always 0, controlled by Pas
    dbTempPasData.setRange( PAS_ADDRESS_DVA_PRIORITY, PAS_ADDRESS_DVA_PRIORITY );
    dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0u );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

	// 3.Set transaction id
	dbTempPasData.setRange( PAS_ADDRESS_STN_TRANSACTION_ID, PAS_ADDRESS_STN_TRANSACTION_ID );
	dbTempPasData.set( PAS_ADDRESS_STN_TRANSACTION_ID, m_usTransationId );
	if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set transaction id" );
		return TRANSACTION_CONNECTION_FAILURE;
	}

    // 4. Set message ID
	//modified by lixiaoxia
	if( 1u == m_vecDvaMessageIds.size() )
	{
		dbTempPasData.setRange( PAS_ADDRESS_STN_MESSAGE_ID, PAS_ADDRESS_STN_MESSAGE_ID );
		std::vector<unsigned short>::iterator itFirst = m_vecDvaMessageIds.begin();
		dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, *itFirst );
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
		dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, (~m_vecDvaMessageIds.size() + 1u ));
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

		dbTempPasData.setRange( PAS_ADDRESS_STN_DWELL_PERIOD, PAS_ADDRESS_STN_DWELL_PERIOD );
		dbTempPasData.set( PAS_ADDRESS_STN_DWELL_PERIOD, m_usDwellPeriod );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set dwell period" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
	}

    // 5. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Write command to start DVA announcement
    const unsigned short usCommandStartDva = 
        m_bWithChime ? PAS_COMMAND_STN_START_CHIME_DVA : PAS_COMMAND_STN_START_DVA;
    dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, usCommandStartDva );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 7. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 8. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 9. Read Command Acknowledge Register
    unsigned short usResult = 0u;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaBroadcast execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandStartDva == usResult )
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

void StartDvaBroadcast::setWithChime( const bool bWithChime )
{
    m_bWithChime = bWithChime;
}

void StartDvaBroadcast::setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds )
{
    m_vecDvaMessageIds = vecMsgIds;
}

void StartDvaBroadcast::setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys )
{
    m_vecPasZoneKeys = vecPasZoneKeys;
}

void StartDvaBroadcast::setTransactionId( const unsigned short usTransationId )
{
	m_usTransationId = usTransationId;
}

void StartDvaBroadcast::setDwellPeriod( const unsigned short usDwellPeriod )
{
	m_usDwellPeriod = usDwellPeriod;
}

}
