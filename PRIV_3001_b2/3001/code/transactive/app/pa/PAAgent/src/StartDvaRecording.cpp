/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StartDvaRecording.cpp $
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
#include "app/pa/PAAgent/src/StartDvaRecording.h"


namespace TA_IRS_App
{

	StartDvaRecording::StartDvaRecording( const bool bAtOcc, ITransactionOwner* pOwner ):
	PasTransaction( bAtOcc, pOwner )
	{
	}

	StartDvaRecording::~StartDvaRecording()
	{
	}

	void StartDvaRecording::setDvaMessageId( const unsigned short usDvaMessageId )
	{
		m_usDvaMessageId = usDvaMessageId;
	}

	void StartDvaRecording::setSourceId( const unsigned short usSourceId )
	{
		m_usSourceId = usSourceId;
	}

	void StartDvaRecording::setMsgType( const unsigned short usMsgType )
	{
		m_usMsgType = usMsgType;
	}

	int StartDvaRecording::executeOccImp( PasConnectionAgency& refConnection )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaRecording" );

		TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

		// 1. Set bank number
		dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
		dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, m_usMsgType );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set bank number" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 2. Set message ID
		dbTempPasData.setRange( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_ADDRESS_OCC_MESSAGE_ID );
		dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, m_usDvaMessageId );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// Set recording time
		dbTempPasData.setRange( PAS_ADDRESS_OCC_RECORDING_TIME, PAS_ADDRESS_OCC_RECORDING_TIME );
		dbTempPasData.set( PAS_ADDRESS_OCC_RECORDING_TIME, 0 );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set recording time" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 3. Reset Execution Register to 0
		if ( !resetExecutionRegister( refConnection ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        // 4. Write command to start DVA Recording
        const unsigned short usCommandDvaRecording = PAS_COMMAND_OCC_START_DVA_RECORDING_BASE | m_usSourceId;
		dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
		dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandDvaRecording );

		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA Recording" );
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaRecording execution ended with result %d", usResult );

		if ( PAS_COMMAND_SUCCESS == usResult )
		{
			return TRANSACTION_SUCCESS;
		}
		else if ( usCommandDvaRecording == usResult )
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

    int StartDvaRecording::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaRecording" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set source number
        dbTempPasData.setRange( PAS_ADDRESS_STN_SOURCE_ID, PAS_ADDRESS_STN_SOURCE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_SOURCE_ID, m_usSourceId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set source number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set bank number
        dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
        dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, m_usMsgType );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set bank number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

		// Set recording time
		dbTempPasData.setRange( PAS_ADDRESS_STN_RECORDING_TIME, PAS_ADDRESS_STN_RECORDING_TIME );
		dbTempPasData.set( PAS_ADDRESS_STN_RECORDING_TIME, 1 );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set recording time" );
			return TRANSACTION_CONNECTION_FAILURE;
		}
        // 3. Set message ID
        dbTempPasData.setRange( PAS_ADDRESS_STN_MESSAGE_ID, PAS_ADDRESS_STN_MESSAGE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, m_usDvaMessageId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 5. Write command to start DVA Recording
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_START_DVA_RECORDING );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA Recording" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 6. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution Register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 7. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 8. Read Command Acknowledge Register
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaRecording execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_STN_START_DVA_RECORDING == usResult )
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