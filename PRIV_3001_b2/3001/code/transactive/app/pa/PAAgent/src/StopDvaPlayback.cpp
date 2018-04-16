/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StopDvaPlayback.cpp $
  * @author:  HuangJian
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
#include "app/pa/PAAgent/src/StopDvaPlayback.h"

namespace TA_IRS_App
{

	StopDvaPlayback::StopDvaPlayback( const bool bAtOcc, ITransactionOwner* pOwner )
		:PasTransaction( bAtOcc, pOwner )
	{
	}

	StopDvaPlayback::~StopDvaPlayback()
	{
	}

	void StopDvaPlayback::setSourceId( const unsigned short usClientNumber )
	{
		m_usSourceId = usClientNumber;
	}

	int StopDvaPlayback::executeOccImp( PasConnectionAgency& refConnection )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaPlayback" );

		TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

		// 1. Set message ID to 0 to stop listening
		dbTempPasData.setRange( PAS_ADDRESS_OCC_PLAYBACK_ID, PAS_ADDRESS_OCC_PLAYBACK_ID );
		dbTempPasData.set( PAS_ADDRESS_OCC_PLAYBACK_ID, 0u );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 2. Reset Execution Register to 0
		if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 3. Write command to start DVA playback
        const unsigned short usCommandPlayback = PAS_COMMAND_OCC_DVA_PLAYBACK_BASE | m_usSourceId;
		dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
		dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandPlayback );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA playback" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 4. Set Execution Register to 1
		if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 5. Read Execution Register until value is 0
		if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
			return TRANSACTION_FAILED;
		}

		// 6. Read Command Acknowledge Register
		unsigned short usResult = 0;
		if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaPlayback execution ended with result %d", usResult );

		if ( PAS_COMMAND_SUCCESS == usResult )
		{
			return TRANSACTION_SUCCESS;
		}
		else if ( usCommandPlayback == usResult )
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

    int StopDvaPlayback::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StopDvaPlayback" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set message ID to 0 to stop listening
        dbTempPasData.setRange( PAS_ADDRESS_STN_PLAYBACK_ID, PAS_ADDRESS_STN_PLAYBACK_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_PLAYBACK_ID, 0u );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set source number
        dbTempPasData.setRange( PAS_ADDRESS_STN_SOURCE_ID, PAS_ADDRESS_STN_SOURCE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_SOURCE_ID, m_usSourceId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set source number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Write command to start DVA announcement
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_DVA_PLAYBACK );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
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
        unsigned short usResult = 0u;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopDvaPlayback execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_STN_DVA_PLAYBACK == usResult )
        {
            // Not defined yet
            return TRANSACTION_FAILED;
        }

        return TRANSACTION_SUCCESS;
    }

}