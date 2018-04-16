/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/DeleteScheduleDva.cpp $
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
#include "app/pa/PAAgent/src/DeleteScheduleDva.h"

namespace TA_IRS_App
{

DeleteScheduleDva::DeleteScheduleDva( const bool bAtOcc, ITransactionOwner* pOwner ) : 
PasTransaction( bAtOcc, pOwner )
{
}

DeleteScheduleDva::~DeleteScheduleDva()
{
}

void DeleteScheduleDva::setScheduleId( const unsigned short usScheduleId )
{
    // No limitation, when out of range, PAS return Not OK.
    m_usScheduleId = usScheduleId;
}

int DeleteScheduleDva::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute DeleteScheduleDva" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Write command to delete scheduled audio message
    unsigned short usCommandDeleteSchedule = PAS_COMMAND_OCC_DELETE_SCHEDULE_BASE | m_usScheduleId;
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommandDeleteSchedule );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to delete scheduled audio message" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 5. Read Command Acknowledge Register
    unsigned short usResult = 0u;
    if ( !readCommandResult( refConnection, usResult ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read command acknowledge register" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DeleteScheduleDva execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( usCommandDeleteSchedule == usResult )
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

int DeleteScheduleDva::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute DeleteScheduleDva" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Set schedule event ID
    dbTempPasData.setRange( PAS_ADDRESS_STN_SCHEDULE_ID, PAS_ADDRESS_STN_SCHEDULE_ID );
    dbTempPasData.set( PAS_ADDRESS_STN_SCHEDULE_ID, m_usScheduleId );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set schedule event ID" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 2. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Write command to delete scheduled audio message
    dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_STN_DELETE_SCHEDULE );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to delete scheduled audio message" );
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

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DeleteScheduleDva execution ended with result %d", usResult );

    if ( PAS_COMMAND_SUCCESS == usResult )
    {
        return TRANSACTION_SUCCESS;
    }
    else if ( PAS_COMMAND_STN_DELETE_SCHEDULE == usResult )
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
