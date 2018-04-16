/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StartBackgroundMusic.cpp $
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
#include "app/pa/PAAgent/src/StartBackgroundMusic.h"


namespace TA_IRS_App
{

    StartBackgroundMusic::StartBackgroundMusic( const bool bAtOcc, ITransactionOwner* pOwner ) :
    PasTransaction( bAtOcc, pOwner ),
    m_vecPasZoneKeys(),
    m_usSourceNumber( 0u )
    {
    }

    StartBackgroundMusic::~StartBackgroundMusic()
    {
    }

    void StartBackgroundMusic::setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys )
    {
        m_vecPasZoneKeys = vecPasZoneKeys;
    }

    void StartBackgroundMusic::setSourceNumber( const unsigned short usSourceNumber )
    {
        m_usSourceNumber = usSourceNumber;
    }

    int StartBackgroundMusic::executeOccImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartBackgroundMusic" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set Matrix Routing Table
        dbTempPasData.setRange( PAS_ADDRESS_OCC_MATRIX_TABLE_START, PAS_ADDRESS_OCC_MATRIX_TABLE_END );
        PasHelpers::getInstance()->formatPasOccStationMatrix( m_vecPasZoneKeys, dbTempPasData );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Matrix Routing Table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset Execution Register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Write command to start DVA announcement
        dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_OCC_START_MUSIC_BROADCAST );

        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Execution Register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 5. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read Execution Register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 6. Read Command Acknowledge Register
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to Read command Acknowledge Register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartBackgroundMusic execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_OCC_START_MUSIC_BROADCAST == usResult)
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

    int StartBackgroundMusic::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute StartBackgroundMusic" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set Matrix Routing Table
        dbTempPasData.setRange( PAS_ADDRESS_STN_MATRIX_TABLE_START, PAS_ADDRESS_STN_MATRIX_TABLE_END );
        PasHelpers::getInstance()->formatPasStnZoneMatrix( m_vecPasZoneKeys, dbTempPasData );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Matrix Routing Table" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set Source number
        dbTempPasData.setRange( PAS_ADDRESS_STN_SOURCE_ID, PAS_ADDRESS_STN_SOURCE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_SOURCE_ID, m_usSourceNumber);
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Source number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset Execution Register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Write command to start DVA announcement
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_START_MUSIC_BROADCAST );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to start DVA announcement" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 5. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set Execution Register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 6. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read Execution Register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 7. Read Command Acknowledge Register
        unsigned short usResult = 0;
        if ( !readCommandResult( refConnection, usResult ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read Command Acknowledge Register" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartBackgroundMusic execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( PAS_COMMAND_STN_START_MUSIC_BROADCAST == usResult)
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