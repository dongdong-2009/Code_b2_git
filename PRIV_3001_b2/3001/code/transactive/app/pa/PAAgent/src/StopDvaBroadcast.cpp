/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StopDvaBroadcast.cpp $
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
#include "app/pa/PAAgent/src/StopDvaBroadcast.h"

namespace TA_IRS_App
{

StopDvaBroadcast::StopDvaBroadcast( const bool bAtOcc, ITransactionOwner* pOwner ) : 
PasTransaction( bAtOcc, pOwner )
{
}

StopDvaBroadcast::~StopDvaBroadcast()
{
}

int StopDvaBroadcast::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaBroadcast" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set Matrix Routing Table
    dbTempPasData.setRange( PAS_ADDRESS_OCC_MATRIX_TABLE_START, PAS_ADDRESS_OCC_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasOccZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Set message ID
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
		PasHelpers::getInstance()->formatMultiMessagesIds(m_vecDvaMessageIds, dbTempPasData);
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Multi-Messages ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
    }

    // 3. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Write command to stop DVA announcement
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_OCC_STOP_DVA );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to stop DVA announcement" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 7. Read Command Acknowledge Register
    unsigned short usResult = 0;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaBroadcast execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( PAS_COMMAND_OCC_STOP_DVA == usResult )
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

int StopDvaBroadcast::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaBroadcast" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set Matrix Routing Table
    dbTempPasData.setRange( PAS_ADDRESS_STN_MATRIX_TABLE_START, PAS_ADDRESS_STN_MATRIX_TABLE_END );
    PasHelpers::getInstance()->formatPasStnZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Set message ID
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
    }

    // 3. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Write command to stop DVA announcement
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_STN_STOP_DVA );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to stop DVA announcement" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 7. Read Command Acknowledge Register
    unsigned short usResult = 0;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaBroadcast execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( PAS_COMMAND_STN_STOP_DVA == usResult )
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

void StopDvaBroadcast::setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds )
{
    m_vecDvaMessageIds = vecMsgIds;
}

void StopDvaBroadcast::setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys )
{
    m_vecPasZoneKeys = vecPasZoneKeys;
}

}
