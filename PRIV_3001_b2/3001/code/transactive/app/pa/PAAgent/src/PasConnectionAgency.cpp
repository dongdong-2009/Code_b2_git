/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasConnectionAgency.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include <algorithm>

#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/IPasConnectionObserver.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"

#define USE_STANDARD_MODBUS_RESTRICTION

namespace TA_IRS_App
{

const long PAS_KEEP_ALIVE_SLEEP_TIMEOUT = 100;
const int PAS_KEEP_ALIVE_BACKUP_CONNECTION_COUNTER_TIMES = 100;
const int MODBUS_READ_BUFF_MAX_SIZE = 0x7D;
const int MODBUS_WRITE_BUFF_MAX_SIZE = 0x7B;
const int COMMUNICATION_RETRY_INTERVAL = 50; // millisecond
const int PAS_DEVICE_ADDRESS = 1;

PasConnectionAgency::PasConnectionAgency() : TA_Base_Core::Thread(),
m_bRunning( false ),
m_bEnable( false ),
m_bForceNotifyObservers( true ),
m_oPrimaryPasConnection( CachedConfig::getInstance()->getPrimaryFEPCAddress(), CachedConfig::getInstance()->getPrimaryFEPCPort() ),
m_oSecondaryPasConnection( CachedConfig::getInstance()->getSecondaryFEPCAddress(), CachedConfig::getInstance()->getSecondaryFEPCPort() ),
m_pCurAvailablePasConnection( NULL ),
m_lstPasConnObservers(),
m_smaControlMode( 0 )
{
    m_oPrimaryPasConnection.setDeviceAddress( PAS_DEVICE_ADDRESS );
    m_oSecondaryPasConnection.setDeviceAddress( PAS_DEVICE_ADDRESS );
}

PasConnectionAgency::~PasConnectionAgency()
{
    clearObservers();

    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }
}

void PasConnectionAgency::terminate()
{
    m_bRunning = false;
    m_smaControlMode.release();
}

void PasConnectionAgency::run()
{
    m_bRunning = true;
    m_bForceNotifyObservers = true;
    int nBackupConnectionCheckCounter = 0;
    ACE_Time_Value atmSleep = ACE_Time_Value::zero;
    atmSleep.msec( PAS_KEEP_ALIVE_SLEEP_TIMEOUT );

    while ( m_bRunning )
    {
        if ( !m_bEnable )
        {
            m_smaControlMode.acquire();

            if ( !m_bRunning )
            {
                break;
            }
        }

        bool bAdvanceStrategy = false;
        if ( PAS_KEEP_ALIVE_BACKUP_CONNECTION_COUNTER_TIMES < nBackupConnectionCheckCounter )
        {
            bAdvanceStrategy = true;
            nBackupConnectionCheckCounter = 0;
        }
        else
        {
            ++nBackupConnectionCheckCounter;
        }

        if ( checkConnections( bAdvanceStrategy ))
        {
            ACE_OS::sleep( atmSleep );
            continue;
        }

        if ( updateCurAvailablePasConnection() )
        {
            m_bForceNotifyObservers = false;
            notifyConnectionObservers();
        }
        else if ( m_bForceNotifyObservers )
        {
            m_bForceNotifyObservers = false;
            notifyConnectionObservers();
        }

        ACE_OS::sleep( atmSleep );
    }
}

bool PasConnectionAgency::checkConnections( bool bAdvanceStrategy )
{
    ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
    // Check whether current available connection is valid
    if ( NULL == m_pCurAvailablePasConnection )
    {
        return false;
    }
    else if ( !m_pCurAvailablePasConnection->isConnected() )
    {
        return false;
    }
    else if ( !m_pCurAvailablePasConnection->isAlive() )
    {
        return false;
    }

    // If current connection is valid, check whether backup connection is need to be connected if possible
    if ( bAdvanceStrategy )
    {
        PasModbusConnection& refBackupConnection = 
            ( &m_oPrimaryPasConnection != m_pCurAvailablePasConnection ) ?  m_oPrimaryPasConnection : m_oSecondaryPasConnection;

        if ( !refBackupConnection.isConnected() )
        {
            refBackupConnection.connect();
        }
        else if ( !refBackupConnection.isAlive() )
        {
            // If backup connection is not alive, reconnect it.
            refBackupConnection.connect();
        }
    }

    // Current available connection is valid, return true
    return true;
}

bool PasConnectionAgency::updateCurAvailablePasConnection()
{
    bool bCheckPrimaryPasConnection = true;
    bool bCheckSecondaryPasConnection = true;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
        if ( &m_oPrimaryPasConnection == m_pCurAvailablePasConnection )
        {
            // Primary Connection must be unavailable when this function is called
            bCheckPrimaryPasConnection = false;
        }
        else if ( &m_oSecondaryPasConnection == m_pCurAvailablePasConnection )
        {
            // Secondary Connection must be unavailable when this function is called
            bCheckSecondaryPasConnection = false;
        }
    }

    PasModbusConnection* pTempAvailablePasConnection = NULL;

    if ( bCheckPrimaryPasConnection )
    {
        if ( m_oPrimaryPasConnection.isAlive() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[Connection] Will use primary connection" );
            pTempAvailablePasConnection = &m_oPrimaryPasConnection;
        }
        else if ( m_oPrimaryPasConnection.connect() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[Connection] Will use primary connection" );
            pTempAvailablePasConnection = &m_oPrimaryPasConnection;
        }
    }

    if (( NULL == pTempAvailablePasConnection ) && bCheckSecondaryPasConnection )
    {
        if ( m_oSecondaryPasConnection.isAlive() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[Connection] Will use secondary connection" );
            pTempAvailablePasConnection = &m_oSecondaryPasConnection;
        }
        else if ( m_oSecondaryPasConnection.connect() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[Connection] Will use secondary connection" );
            pTempAvailablePasConnection = &m_oSecondaryPasConnection;
        }
    }

    // Update current available connection
    bool bConnectionUpdated = false;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
        if ((( NULL == pTempAvailablePasConnection ) && ( NULL != m_pCurAvailablePasConnection )) || 
            (( NULL != pTempAvailablePasConnection ) && ( NULL == m_pCurAvailablePasConnection )))
        {
            bConnectionUpdated = true;
        }
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
        m_pCurAvailablePasConnection = pTempAvailablePasConnection;
    }

    return bConnectionUpdated;
}

void PasConnectionAgency::notifyConnectionObservers()
{
    bool bConnectionAvailable = false;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
        bConnectionAvailable = ( NULL != m_pCurAvailablePasConnection );
    }

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxPasConnObservsers );
    // Notify connection observers
    for ( std::list<IPasConnectionObserver*>::iterator itObsLoop = m_lstPasConnObservers.begin(); 
        m_lstPasConnObservers.end() != itObsLoop; ++itObsLoop )
    {
        IPasConnectionObserver* pPasObserver = *itObsLoop;
        if ( NULL != pPasObserver )
        {
            pPasObserver->connectionUpdated( bConnectionAvailable );
        }
    }
}

void PasConnectionAgency::enable( const bool bEnable )
{
    if ( m_bEnable == bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    if ( bEnable )
    {
        m_oPrimaryPasConnection.enable();
        m_oSecondaryPasConnection.enable();
        m_smaControlMode.release();
    }
    else
    {
        m_bForceNotifyObservers = true;
        m_oPrimaryPasConnection.disable();
        m_oSecondaryPasConnection.disable();
    }
}

bool PasConnectionAgency::writeToPas( const TA_Base_Bus::DataBlock<unsigned short>& refData )
{
    ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
    if ( NULL == m_pCurAvailablePasConnection )
    {
        return false;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Write modbus data [%d, %d] to PAS", refData.start(), refData.end() );

    bool bSuccess = false;

    {
        ACE_Guard<ACE_RW_Mutex> gurad( m_mtxPasConnReadWrite );
        // If defined MACRO, then we apply the standard Modbus restriction
        #ifdef USE_STANDARD_MODBUS_RESTRICTION
        if ( MODBUS_WRITE_BUFF_MAX_SIZE < refData.length() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Start to write large block [%d, %d]", refData.start(), refData.end() );

            int nStart = refData.start();
            TA_Base_Bus::DataBlock<unsigned short> dbSplit;
            dbSplit.setStartAndLength( nStart, MODBUS_WRITE_BUFF_MAX_SIZE );
            const int TEMP_RETRY_TIMES = 2;

            // Loop each block
            while ( dbSplit.end() <= refData.end() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Start to write split block [%d, %d]", dbSplit.start(), dbSplit.end() );

                refData.getDataBlock( dbSplit );

                bSuccess = m_pCurAvailablePasConnection->writeRegisterWithRetry( dbSplit, TEMP_RETRY_TIMES );
                if ( !bSuccess )
                {
                    break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Write split block [%d, %d] successfully", dbSplit.start(), dbSplit.end() );

                nStart = dbSplit.end() + 1;
                dbSplit.setStartAndLength( nStart, MODBUS_WRITE_BUFF_MAX_SIZE );
            }

            // Split to the last block
            if ( nStart <= refData.end() )
            {
                dbSplit.setRange( nStart, refData.end() );
                refData.getDataBlock( dbSplit );
                bSuccess = m_pCurAvailablePasConnection->writeRegisterWithRetry( dbSplit, TEMP_RETRY_TIMES );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Write large block [%d, %d] successfully", refData.start(), refData.end() );
        }
        else
        #endif
        {
            bSuccess = m_pCurAvailablePasConnection->writeRegisterWithRetry( refData, 0 );
        }
    }

    return bSuccess;
}

bool PasConnectionAgency::readFromPas( TA_Base_Bus::DataBlock<unsigned short>& refData )
{
    ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxCurAvailablePasConn );
    if ( NULL == m_pCurAvailablePasConnection )
    {
        return false;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Read modbus data [%d, %d] from PAS", refData.start(), refData.end() );

    bool bSuccess = false;

    {
        ACE_Guard<ACE_RW_Mutex> gurad( m_mtxPasConnReadWrite );
        // If defined MACRO, then we apply the standard Modbus restriction
        #ifdef USE_STANDARD_MODBUS_RESTRICTION
        if ( MODBUS_READ_BUFF_MAX_SIZE < refData.length() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Start to read large block [%d, %d]", refData.start(), refData.end() );

            int nStart = refData.start();
            TA_Base_Bus::DataBlock<unsigned short> dbSplit;
            dbSplit.setStartAndLength( nStart, MODBUS_READ_BUFF_MAX_SIZE );
            const int TEMP_RETRY_TIMES = 2;

            // Loop each block
            while ( dbSplit.end() <= refData.end() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Start to read split block [%d, %d]", dbSplit.start(), dbSplit.end() );

                bSuccess = m_pCurAvailablePasConnection->readRegisterWithRetry( dbSplit, TEMP_RETRY_TIMES );
                if ( !bSuccess )
                {
                    break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Read split block [%d, %d] successfully", dbSplit.start(), dbSplit.end() );

                refData.setDataByDataBlock( dbSplit );

                nStart = dbSplit.end() + 1;
                dbSplit.setStartAndLength( nStart, MODBUS_READ_BUFF_MAX_SIZE );
            }

            // Split to the last block
            if ( nStart <= refData.end() )
            {
                dbSplit.setRange( nStart, refData.end() );
                bSuccess = m_pCurAvailablePasConnection->readRegisterWithRetry( dbSplit, TEMP_RETRY_TIMES );
                refData.setDataByDataBlock( dbSplit );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Performance] Read large block [%d, %d] successfully", refData.start(), refData.end() );
        }
        else
        #endif
        {
            bSuccess = m_pCurAvailablePasConnection->readRegisterWithRetry( refData, 0 );
        }
    }

    return bSuccess;
}

void PasConnectionAgency::registerConnectionStatusObserver( IPasConnectionObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        return;
    }

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxPasConnObservsers );

    if ( m_lstPasConnObservers.end() == std::find( m_lstPasConnObservers.begin(), m_lstPasConnObservers.end(), pObserver ))
    {
        m_lstPasConnObservers.push_back( pObserver );
    }
}

void PasConnectionAgency::deregisterConnectionStatusObserver( IPasConnectionObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        return;
    }

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxPasConnObservsers );

    std::list<IPasConnectionObserver*>::iterator itFound = std::find( m_lstPasConnObservers.begin(), m_lstPasConnObservers.end(), pObserver );

    if ( m_lstPasConnObservers.end() != itFound )
    {
        m_lstPasConnObservers.erase( itFound );
    }
}

bool PasConnectionAgency::writeToPasWithRetry( const TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes )
{
    ACE_Time_Value atmRetryInterval = ACE_Time_Value::zero;
    atmRetryInterval.msec( COMMUNICATION_RETRY_INTERVAL );
    int nRetryLoop = 0;

    bool bSuccess = true;
    while ( !writeToPas( refData ))
    {
        ++nRetryLoop;
        if ( nTryTimes <= nRetryLoop )
        {
            bSuccess = false;
            break;
        }

        ACE_OS::sleep( atmRetryInterval );
    }

    return bSuccess;
}

bool PasConnectionAgency::readFromPasWithRetry( TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes )
{
    ACE_Time_Value atmRetryInterval = ACE_Time_Value::zero;
    atmRetryInterval.msec( COMMUNICATION_RETRY_INTERVAL );
    int nRetryLoop = 0;

    bool bSuccess = true;
    while ( !readFromPas( refData ))
    {
        ++nRetryLoop;
        if ( nTryTimes <= nRetryLoop )
        {
            bSuccess = false;
            break;
        }

        ACE_OS::sleep( atmRetryInterval );
    }

    return bSuccess;
}


void PasConnectionAgency::clearObservers()
{
    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxPasConnObservsers );

    m_lstPasConnObservers.clear();
}

}
