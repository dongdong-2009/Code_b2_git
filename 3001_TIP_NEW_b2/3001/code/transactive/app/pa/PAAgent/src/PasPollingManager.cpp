/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasPollingManager.cpp $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/11/19 14:38:40 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/Guard_T.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasPollingWorker.h"
#include "app/pa/PAAgent/src/PasPollingManager.h"

namespace TA_IRS_App
{

PasPollingManager::PasPollingManager( PasConnectionAgency& refPasConnection ) : 
IPasConnectionObserver(),
m_refPasConnection( refPasConnection ),
m_lstPasPollingWorkers(),
m_mtxPollingWorkerLock(),
m_bEnable( false )
{
    init();
}

PasPollingManager::~PasPollingManager()
{
    clean();
}

void PasPollingManager::createPollingTask( const int nStartAddress, const int nEndAddress, long lPollingInterval )
{
    if (( 0 > nStartAddress ) || ( nEndAddress < nStartAddress ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Invalid input parameters [%d, %d] while calling PasPollingManager::createPollingTask", 
            nStartAddress, nEndAddress );
        return;
    }

    if ( PAS_POLLING_MIN_INTERVAL > lPollingInterval )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Invalid polling interval [%d] while createPollingTask, fixed to %d", lPollingInterval, PAS_POLLING_MIN_INTERVAL );
        lPollingInterval = PAS_POLLING_MIN_INTERVAL;
    }

    PasPollingWorker oTempWorker( m_refPasConnection );

    // Add polling worker
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        m_lstPasPollingWorkers.push_back( oTempWorker );
        try
        {
            PasPollingWorker& refNewWorker = m_lstPasPollingWorkers.back();
            refNewWorker.initPollingSetting( nStartAddress, nEndAddress, lPollingInterval );
            refNewWorker.start();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception when refer to back of list of polling workers" );
        }
    }
}

void PasPollingManager::registerPollingObserver( IPasPollingObserver& refObserver, const int nStartAddress, const int nEndAddress )
{
    bool bRegistered = false;

    // Register polling observer for all polling worker if in polling range
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        for ( std::list<PasPollingWorker>::iterator itLoop = m_lstPasPollingWorkers.begin(); 
                m_lstPasPollingWorkers.end() != itLoop; ++ itLoop )
        {
            if ( itLoop->isInPollingRange( nStartAddress, nEndAddress ))
            {
                bRegistered = true;
                itLoop->registerPollingObserver( refObserver );
            }
        }
    }

    if ( !bRegistered )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Observer %d can't register monitor address [%d, %d]", 
            reinterpret_cast<long>(&refObserver), nStartAddress, nEndAddress );
    }
}

void PasPollingManager::deregisterPollingObserver( IPasPollingObserver& refObserver, const int nStartAddress, const int nEndAddress )
{
    bool bDeregistered = false;

    // De-register polling observer for all polling worker if in polling range
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        for ( std::list<PasPollingWorker>::iterator itLoop = m_lstPasPollingWorkers.begin(); 
                m_lstPasPollingWorkers.end() != itLoop; ++ itLoop )
        {
            if ( itLoop->isInPollingRange( nStartAddress, nEndAddress ))
            {
                bDeregistered = true;
                itLoop->deregisterPollingObserver( refObserver );
            }
        }
    }

    if ( !bDeregistered )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Observer %d can't de-register monitor address [%d, %d]", 
            reinterpret_cast<long>(&refObserver), nStartAddress, nEndAddress );
    }
}

void PasPollingManager::init()
{
    m_refPasConnection.registerConnectionStatusObserver( this );

    createPollingTask( PAS_POLLING_SYSTEM_FAULT, PAS_POLLING_SYSTEM_FAULT,
        CachedConfig::getInstance()->getSystemFaultPollingIntervalInMillisecond() );

    createPollingTask( PAS_POLLING_ZONE_COUNTER_START, PAS_POLLING_ZONE_COUNTER_END, 
        CachedConfig::getInstance()->getPasZonePollingIntervalInMillisecond() );

    createPollingTask( PAS_POLLING_DVA_LIB_COUNTER, PAS_POLLING_DVA_LIB_COUNTER, 
        CachedConfig::getInstance()->getDvaLibPollingIntervalInMillisecond() );

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        createPollingTask( PAS_POLLING_OCC_FIREALARM_START, PAS_POLLING_OCC_FIREALARM_END, 
            CachedConfig::getInstance()->getFireAlarmPollingIntervalInMillisecond() );

        createPollingTask( PAS_POLLING_SYSTEM_FAULT_START, PAS_POLLING_SYSTEM_FAULT_END,
            CachedConfig::getInstance()->getSystemFaultPollingIntervalInMillisecond() );
    }
    else
    {
        createPollingTask( PAS_POLLING_STN_FIREALARM, PAS_POLLING_STN_FIREALARM, 
            CachedConfig::getInstance()->getFireAlarmPollingIntervalInMillisecond() );
    }
}

void PasPollingManager::enable( bool bEnable )
{
    if ( m_bEnable == bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    // Enable/disable polling worker
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        for ( std::list<PasPollingWorker>::iterator itLoop = m_lstPasPollingWorkers.begin(); 
                m_lstPasPollingWorkers.end() != itLoop; ++ itLoop )
        {
            itLoop->enable( bEnable );
        }
    }
}

void PasPollingManager::connectionUpdated( bool bConnectionAvailable )
{
    if ( !m_bEnable )
    {
        return;
    }

    // Enable/disable polling worker
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        for ( std::list<PasPollingWorker>::iterator itLoop = m_lstPasPollingWorkers.begin(); 
                m_lstPasPollingWorkers.end() != itLoop; ++ itLoop )
        {
            itLoop->enable( bConnectionAvailable );
        }
    }
}

void PasPollingManager::clean()
{
    m_refPasConnection.deregisterConnectionStatusObserver( this );

    // Clean polling worker
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxPollingWorkerLock );

        for ( std::list<PasPollingWorker>::iterator itLoop = m_lstPasPollingWorkers.begin(); 
            m_lstPasPollingWorkers.end() != itLoop; ++ itLoop )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == itLoop->getCurrentState() )
            {
                itLoop->terminateAndWait();
            }
        }
        m_lstPasPollingWorkers.clear();
    }
}

}
