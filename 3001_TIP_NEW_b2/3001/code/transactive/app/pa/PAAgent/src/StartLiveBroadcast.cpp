/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StartLiveBroadcast.cpp $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/StartLiveBroadcast.h"


namespace TA_IRS_App
{

	StartLiveBroadcast::StartLiveBroadcast( const bool bAtOcc, ITransactionOwner* pOwner ):
	PasTransaction( bAtOcc, pOwner )
	{
	}

	StartLiveBroadcast::~StartLiveBroadcast()
	{
	}

	void StartLiveBroadcast::setWithChime( const bool bWithChime )
	{
		m_bWithChime = bWithChime;
	}

	void StartLiveBroadcast::setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys )
	{
		m_vecPasZoneKeys = vecPasZoneKeys;
    }

    void StartLiveBroadcast::setSourceId( const unsigned short usSourceId )
    {
        m_usSourceId = usSourceId;
    }
	void StartLiveBroadcast::setTransactionId( const unsigned short usTransactionId )
	{
		m_usTransationId = usTransactionId;
	}

	int StartLiveBroadcast::executeOccImp( PasConnectionAgency& refConnection )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartLiveBroadcast" );

		TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

		// 1. Set Matrix Routing Table
		dbTempPasData.setRange( PAS_ADDRESS_OCC_MATRIX_TABLE_START, PAS_ADDRESS_OCC_MATRIX_TABLE_END );
		PasHelpers::getInstance()->formatPasOccZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set matrix routing table" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 2. Set priority, always 0, controlled by Pas
		dbTempPasData.setRange( PAS_ADDRESS_DVA_PRIORITY, PAS_ADDRESS_DVA_PRIORITY );
		dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0 );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority, always 0, controlled by Pas" );
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

		if ( m_bWithChime )
		{
			// 4. Set Chime Number
			dbTempPasData.setRange( PAS_ADDRESS_OCC_CHIME, PAS_ADDRESS_OCC_CHIME );
			dbTempPasData.set( PAS_ADDRESS_OCC_CHIME, 0u );
			if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set chime number" );
				return TRANSACTION_CONNECTION_FAILURE;
			}
		}

		// 5. Reset Execution Register to 0
		if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

		// 6. Write command to start live announcement
        const unsigned short usCommandLive = m_bWithChime ? 
            ( PAS_COMMAND_OCC_CHIME_LIVE_BASE | m_usSourceId ) : ( PAS_COMMAND_OCC_LIVE_BASE | m_usSourceId );
		dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
		dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandLive );
		if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start live announcement" );
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
		unsigned short usResult = 0;
		if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
			return TRANSACTION_CONNECTION_FAILURE;
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartLiveBroadcast execution ended with result %d", usResult );

		if ( PAS_COMMAND_SUCCESS == usResult )
		{
			return TRANSACTION_SUCCESS;
		}
		else if ( usCommandLive == usResult )
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

    int StartLiveBroadcast::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartLiveBroadcast" );

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
        dbTempPasData.set( PAS_ADDRESS_DVA_PRIORITY, 0 );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set priority, always 0, controlled by Pas" );
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

        if ( m_bWithChime )
        {
            // 4. Set Chime Number
            dbTempPasData.setRange( PAS_ADDRESS_STN_CHIME, PAS_ADDRESS_STN_CHIME );
            dbTempPasData.set( PAS_ADDRESS_STN_CHIME, 0u );
            if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set chime number" );
                return TRANSACTION_CONNECTION_FAILURE;
            }
        }

        // 5. Set source number
        dbTempPasData.setRange( PAS_ADDRESS_STN_SOURCE_ID, PAS_ADDRESS_STN_SOURCE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_SOURCE_ID,  m_usSourceId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set source number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 6. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution Register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 7. Write command to start DVA announcement
        const unsigned short usCommandStartLive = m_bWithChime ? PAS_COMMAND_STN_CHIME_LIVE : PAS_COMMAND_STN_LIVE;
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER,  usCommandStartLive );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 8. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 9. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 10. Read Command Acknowledge Register
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }


        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartLiveBroadcast execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( usCommandStartLive == usResult )
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