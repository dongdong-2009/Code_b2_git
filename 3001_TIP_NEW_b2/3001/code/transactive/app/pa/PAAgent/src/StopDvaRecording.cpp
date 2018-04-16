/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StopDvaRecording.cpp $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/26 10:45:17 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/StopDvaRecording.h"


namespace TA_IRS_App
{

	StopDvaRecording::StopDvaRecording( const bool bAtOcc, ITransactionOwner* pOwner ):
	PasTransaction( bAtOcc, pOwner )
	{
	}

	StopDvaRecording::~StopDvaRecording()
	{
	}

	void StopDvaRecording::setDvaMessageId( const unsigned short usDvaMessageId )
	{
		m_usDvaMessageId = usDvaMessageId;
	}

	void StopDvaRecording::setMsgType( const unsigned short usMsgType )
	{
		m_usMsgType = usMsgType;
	}

	void StopDvaRecording::setSourceId( const unsigned short usSourceId )
	{
		m_usSourceId = usSourceId;
	}

	int StopDvaRecording::executeOccImp( PasConnectionAgency& refConnection )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaRecording" );

		TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

		// 1. Set bank number
		dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
		dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, m_usMsgType );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 2. Reset Execution Register to 0
		if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        // 3. Write command to start DVA Recording
        const unsigned short usCommandDvaRecording = PAS_COMMAND_OCC_STOP_DVA_RECORDING_BASE | m_usSourceId;
		dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
		dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandDvaRecording );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 4. Set Execution Register to 1
		if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 5. Read Execution Register until value is 0
		if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_FAILED;
		}

		// 6. Read Command Acknowledge Register
		unsigned short usResult = 0;
		if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaRecording execution ended with result %d", usResult );

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

    int StopDvaRecording::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaRecording" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set source number
        dbTempPasData.setRange( PAS_ADDRESS_STN_SOURCE_ID, PAS_ADDRESS_STN_SOURCE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_SOURCE_ID, m_usSourceId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set bank number
        dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
        dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, m_usMsgType );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Write command to start DVA Recording
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_STOP_DVA_RECORDING );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 5. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 6. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_FAILED;
        }

        // 7. Read Command Acknowledge Register
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaRecording execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_STN_STOP_DVA_RECORDING == usResult )
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