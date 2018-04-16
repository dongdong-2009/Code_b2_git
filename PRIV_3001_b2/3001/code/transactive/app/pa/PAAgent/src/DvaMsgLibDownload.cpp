/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/DvaMsgLibDownload.cpp $
  * @author:  HuangQi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"

#include "bus/modbus_comms/src/DataBlock.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/IPasDataProcessor.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/DvaMsgLibDownload.h"

namespace TA_IRS_App
{

DvaMsgLibDownload::DvaMsgLibDownload( const bool bAtOcc, IPasSerialDataProcessor& refProcessor ) : 
PasTransaction( bAtOcc, NULL ),
m_refProcessor( refProcessor )
{
}

DvaMsgLibDownload::~DvaMsgLibDownload()
{
}

int DvaMsgLibDownload::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute DvaMsgLibDownload" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Triggered from polling

    // 2. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Write command to read first 6 messages
    dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_DVA_LIB_NEW_DOWNLOAD );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to read first 6 messages" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_FAILED );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 6. Read details of the first 6 messages
    dbTempPasData.setRange( PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START, PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END );
    if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read details of the messages" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6.1. Notify process to update DVA library
    if ( !m_refProcessor.processNewData( dbTempPasData ))
    {
        // continue anyway according to current protocol
    }

    do 
    {
        // 7. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 8. Write command to read next 6 messages
        dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_DVA_LIB_NEXT_DOWNLOAD );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to read next 6 messages" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 9. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 10. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_FAILED );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 11. Read details of the next 6 messages
        dbTempPasData.setRange( PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START, PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END );
        if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read details of the messages" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 12. Repeat steps 7 to 11 until message ID = 0. Message ID = 0 means the end of the message list
    } while ( m_refProcessor.processRemainData( dbTempPasData ));

    // 13. Upon reading message ID = 0, read message bank CRC number to update the message Library Version
    dbTempPasData.setRange( PAS_ADDRESS_OCC_DVA_LIB_VERSION_START, PAS_ADDRESS_OCC_DVA_LIB_VERSION_END );
    if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read bank CRC number" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DvaMsgLibDownload execution ended " );

    // 13.1. Notify process to update DVA library version
    m_refProcessor.processFinalData( dbTempPasData );

    return TRANSACTION_SUCCESS;
}

int DvaMsgLibDownload::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute DvaMsgLibDownload" );

    TA_Base_Bus::DataBlock<unsigned short> dbTempPasData;

    // 1. Triggered from polling

    // 2. Reset Execution Register to 0
    if ( !resetExecutionRegister( refConnection ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 3. Write command to read first 6 messages
    dbTempPasData.setRange( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_ADDRESS_STN_COMMAND_REGISTER );
    dbTempPasData.set( PAS_ADDRESS_STN_COMMAND_REGISTER, PAS_COMMAND_DVA_LIB_NEW_DOWNLOAD );
    if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to read first 6 messages" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 4. Set Execution Register to 1
    if ( !markExecutionRegister( refConnection ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 5. Read Execution Register until value is 0
    if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_FAILED );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
        return TRANSACTION_FAILED;
    }

    // 6. Read details of the first 6 messages
    dbTempPasData.setRange( PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START, PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END );
    if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read details of the messages" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    // 6.1. Notify process to update DVA library
    if ( !m_refProcessor.processNewData( dbTempPasData ))
    {
        // continue anyway according to current protocol
    }

    do 
    {
        // 7. Reset Execution Register to 0
        if ( !resetExecutionRegister( refConnection ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to reset execution register to 0" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 8. Write command to read next 6 messages, address is special, same as OCC's
        dbTempPasData.setRange( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_ADDRESS_OCC_COMMAND_REGISTER );
        dbTempPasData.set( PAS_ADDRESS_OCC_COMMAND_REGISTER, PAS_COMMAND_DVA_LIB_NEXT_DOWNLOAD );
        if ( !refConnection.writeToPasWithRetry( dbTempPasData ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to write command to read next 6 messages" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 9. Set Execution Register to 1
        if ( !markExecutionRegister( refConnection ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set execution register to 1" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 10. Read Execution Register until value is 0
        if ( !waitExecutionAcknowledged( refConnection, PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_FAILED );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read execution register until value is 0" );
            return TRANSACTION_FAILED;
        }

        // 11. Read details of the next 6 messages
        dbTempPasData.setRange( PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START, PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END );
        if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
        {
            m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read details of the messages" );
            return TRANSACTION_CONNECTION_FAILURE;
        }

        // 12. Repeat steps 7 to 11 until message ID = 0. Message ID = 0 means the end of the message list
    } while ( m_refProcessor.processRemainData( dbTempPasData ));

    // 13. Upon reading message ID = 0, read message bank CRC number to update the message Library Version
    dbTempPasData.setRange( PAS_ADDRESS_STN_DVA_LIB_VERSION_START, PAS_ADDRESS_STN_DVA_LIB_VERSION_END );
    if ( !refConnection.readFromPasWithRetry( dbTempPasData ))
    {
        m_refProcessor.serialProcessFailed( TRANSACTION_CONNECTION_FAILURE );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read bank CRC number" );
        return TRANSACTION_CONNECTION_FAILURE;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DvaMsgLibDownload execution ended " );

    // 13.1. Notify process to update DVA library version
    m_refProcessor.processFinalData( dbTempPasData );

    return TRANSACTION_SUCCESS;
}

}
