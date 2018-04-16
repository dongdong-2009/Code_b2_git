/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StartDvaPlayback.cpp $
  * @author:  HuangQi
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/StartDvaPlayback.h"


namespace TA_IRS_App
{

	StartDvaPlayback::StartDvaPlayback( const bool bAtOcc, ITransactionOwner* pOwner )
		:PasTransaction( bAtOcc, pOwner )
	{

	}

	StartDvaPlayback::~StartDvaPlayback()
	{

	}

	void StartDvaPlayback::setDvaMessageId( const unsigned short usDvaMessageId )
	{
		m_usDvaMessageId = usDvaMessageId;
    }

    void StartDvaPlayback::setSourceId( const unsigned short usSourceId )
    {
        m_usSourceId = usSourceId;
    }

	int StartDvaPlayback::executeOccImp( PasConnectionAgency& refConnection )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaPlayback" );

		TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

		// 1. Set message ID
		dbTempPasData.setRange( PAS_ADDRESS_OCC_PLAYBACK_ID, PAS_ADDRESS_OCC_PLAYBACK_ID );
		dbTempPasData.set( PAS_ADDRESS_OCC_PLAYBACK_ID, m_usDvaMessageId );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message ID" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 2. Reset Execution Register to 0
		if ( !resetExecutionRegister( refConnection ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset Execution register to 0" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        // 3. Write command to start DVA announcement
        const unsigned short usCommandDvaPlayback = PAS_COMMAND_OCC_DVA_PLAYBACK_BASE | m_usSourceId;
		dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
		dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandDvaPlayback );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaPlayback execution ended with result %d", usResult );

		if ( PAS_COMMAND_SUCCESS == usResult )
		{
			return TRANSACTION_SUCCESS;
		}
		else if ( usCommandDvaPlayback == usResult )
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

    int StartDvaPlayback::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartDvaPlayback" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set message ID
        dbTempPasData.setRange( PAS_ADDRESS_STN_PLAYBACK_ID, PAS_ADDRESS_STN_PLAYBACK_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_PLAYBACK_ID, m_usDvaMessageId );
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
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartDvaPlayback execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_STN_DVA_PLAYBACK == usResult )
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