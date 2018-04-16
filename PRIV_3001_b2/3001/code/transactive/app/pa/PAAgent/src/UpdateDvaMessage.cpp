/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/UpdateDvaMessage.cpp $
  * @author:  HuangQi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/UpdateDvaMessage.h"

namespace TA_IRS_App
{

    UpdateDvaMessage::UpdateDvaMessage( const bool bAtOcc, ITransactionOwner* pOwner ) :
    PasTransaction( bAtOcc, pOwner )
    {
    }

    UpdateDvaMessage::~UpdateDvaMessage()
    {
    }

    int UpdateDvaMessage::executeOccImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute UpdateDvaMessage" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set bank number
        const unsigned short usNewType = m_bToBeDeleted ? m_usDvaOrgMsgType : m_usUpdateMessageType;
        dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
        dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, usNewType );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set bank number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set message number
        dbTempPasData.setRange( PAS_ADDRESS_OCC_MESSAGE_ID, PAS_ADDRESS_OCC_MESSAGE_ID );
        dbTempPasData.set( PAS_ADDRESS_OCC_MESSAGE_ID, m_usDvaMsgId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Write usCommand to start DVA announcement
        unsigned short usCommand = 
            m_bToBeDeleted ? PAS_COMMAND_OCC_DELETE_DVA : PAS_COMMAND_OCC_CHANGE_DVA_TYPE;
        dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, usCommand );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write usCommand to start DVA announcement" );
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "UpdateDvaMessage execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( usCommand == usResult )
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

    int UpdateDvaMessage::executeStnImp( PasConnectionAgency& refConnection )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute UpdateDvaMessage" );

        TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

        // 1. Set bank number
        const unsigned short usNewType = m_bToBeDeleted ? m_usDvaOrgMsgType : m_usUpdateMessageType;
        dbTempPasData.setRange( PAS_ADDRESS_DVA_BANK_NUMBER, PAS_ADDRESS_DVA_BANK_NUMBER );
        dbTempPasData.set( PAS_ADDRESS_DVA_BANK_NUMBER, usNewType );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set bank number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 2. Set message number
        dbTempPasData.setRange( PAS_ADDRESS_STN_MESSAGE_ID, PAS_ADDRESS_STN_MESSAGE_ID );
        dbTempPasData.set( PAS_ADDRESS_STN_MESSAGE_ID, m_usDvaMsgId );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set message number" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 3. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 4. Write usCommand to start DVA announcement
        unsigned short usCommand = 
            m_bToBeDeleted ? PAS_COMMAND_STN_DELETE_DVA : PAS_COMMAND_STN_CHANGE_DVA_TYPE;
        dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, usCommand );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write usCommand to start DVA announcement" );
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "UpdateDvaMessage execution ended with result %d", usResult );

        if ( PAS_COMMAND_SUCCESS == usResult )
        {
            return TRANSACTION_SUCCESS;
        }
        else if ( usCommand == usResult )
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

    void UpdateDvaMessage::setUpdateMessageType( unsigned short usUpdateMessageType )
    {
        m_usUpdateMessageType = usUpdateMessageType;
    }

    void UpdateDvaMessage::setOriginalMessageType( const unsigned short usOrgMsgType )
    {
        m_usDvaOrgMsgType = usOrgMsgType;
    }

    void UpdateDvaMessage::setDvaMsgId( const unsigned short usDvaMsgId )
    {
        m_usDvaMsgId = usDvaMsgId;
    }

    void UpdateDvaMessage::setToBeDeleted( const bool bToBeDeleted )
    {
        m_bToBeDeleted = bToBeDeleted;
    }

}