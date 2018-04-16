/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AcknowledgeFireAlarm.cpp $
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
#include "app/pa/PAAgent/src/AcknowledgeFireAlarm.h"


namespace TA_IRS_App
{

AcknowledgeFireAlarm::AcknowledgeFireAlarm( const bool bAtOcc ) : 
PasTransaction( bAtOcc, NULL )
{
}

AcknowledgeFireAlarm::~AcknowledgeFireAlarm()
{
}

void AcknowledgeFireAlarm::setStationId( const unsigned short usStationId )
{
    // No use for station command
    m_usStationId = usStationId;
}

int AcknowledgeFireAlarm::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute AcknowledgeFireAlarm" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set remote number
    dbTempPasData.setRange( PAS_ADDRESS_OCC_REMOTE_NUMBER, PAS_ADDRESS_OCC_REMOTE_NUMBER );
    dbTempPasData.set( PAS_ADDRESS_OCC_REMOTE_NUMBER, PAS_REMOTE_NUMBER_FIRE_ALARM );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set remote number" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Set message number
    dbTempPasData.setRange( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_ADDRESS_OCC_MESSAGE_ID );
    dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_MESSAGE_NUMBER_FIRE_ALARM );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set remote number" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Write command to acknowledge fire alarm
    const unsigned short usCommandAckFireAlarm = PAS_COMMAND_OCC_ACKNOWLEDEG_FIRE_ALARM_BASE | m_usStationId;
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandAckFireAlarm );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to acknowledge fire alarm" );
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

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AcknowledgeFireAlarm execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandAckFireAlarm == usResult )
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

int AcknowledgeFireAlarm::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute AcknowledgeFireAlarm" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set remote number
    dbTempPasData.setRange( PAS_ADDRESS_STN_REMOTE_NUMBER, PAS_ADDRESS_STN_REMOTE_NUMBER );
    dbTempPasData.set( PAS_ADDRESS_STN_REMOTE_NUMBER, PAS_REMOTE_NUMBER_FIRE_ALARM );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set remote number" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Write command to acknowledge fire alarm
    dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_ACKNOWLEDEG_FIRE_ALARM );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to acknowledge fire alarm" );
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
        return TRANSACTION_FAILED;
    }

    // 6. Read Command Acknowledge Register
    unsigned short usResult = 0;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AcknowledgeFireAlarm execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( PAS_COMMAND_STN_ACKNOWLEDEG_FIRE_ALARM == usResult )
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
