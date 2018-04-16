/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasPollingWorker.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include <algorithm>

#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/IPasPollingObserver.h"
#include "app/pa/PAAgent/src/PasPollingWorker.h"

namespace TA_IRS_App
{

PasPollingWorker::PasPollingWorker( PasConnectionAgency& refPasConnection ) : 
TA_Base_Core::Thread(),
m_refPasConnectionAgentcy( refPasConnection ),
m_bRunning( false ),
m_bEnable( false ),
m_bInited( false ),
m_dbPolledData(),
m_ulPolledTimeStamp( 0u ),
m_lPollingIntervalMilSec( 0 ),
m_lstPollingObservers(),
m_smaEnableLock( 0 ),
m_mtxObserversLock()
{
}

PasPollingWorker::PasPollingWorker( const PasPollingWorker& refSrc ) : 
TA_Base_Core::Thread(),
m_refPasConnectionAgentcy( refSrc.m_refPasConnectionAgentcy ),
m_bRunning( refSrc.m_bRunning ),
m_bEnable( refSrc.m_bEnable ),
m_bInited( false ),
m_dbPolledData(),
m_ulPolledTimeStamp( refSrc.m_ulPolledTimeStamp ),
m_lPollingIntervalMilSec( refSrc.m_lPollingIntervalMilSec ),
m_lstPollingObservers(),
m_smaEnableLock( 0 ),
m_mtxObserversLock()
{
}

PasPollingWorker::~PasPollingWorker()
{
}

void PasPollingWorker::run()
{
    if ( !m_bInited )
    {
        return;
    }

    m_bRunning = true;
    ACE_Time_Value atmSleep = ACE_Time_Value::zero;
    atmSleep.msec( m_lPollingIntervalMilSec );

    while ( m_bRunning )
    {
        if ( !m_bEnable )
        {
            m_smaEnableLock.acquire();

            if ( !m_bRunning )
            {
                break;
            }
        }

        if ( polling() )
        {
            notifyObservers();
        }

        ACE_OS::sleep( atmSleep );
    }
}

void PasPollingWorker::terminate()
{
    m_bRunning = false;
    m_smaEnableLock.release();
}

bool PasPollingWorker::polling()
{
    if ( 0 == m_dbPolledData.length() )
    {
        return false;
    }

    TA_Base_Bus::DataBlock<unsigned short> dbTempPollingData( m_dbPolledData.start(), m_dbPolledData.end() );

    if ( !m_refPasConnectionAgentcy.readFromPasWithRetry( dbTempPollingData ))
    {
        return false;
    }

    // Refresh polled data and timestamp when data updated or first time polling
    if ( dbTempPollingData.operator !=( m_dbPolledData ) || ( 0 == m_ulPolledTimeStamp ))
    {
        m_ulPolledTimeStamp = ACE_OS::gettimeofday().msec();
        m_dbPolledData = dbTempPollingData;
        return true;
    }

    return false;
}

void PasPollingWorker::enable( const bool bEnable )
{
    if ( m_bEnable == bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    if ( bEnable )
    {
        // Force fresh data and notify observer
        m_ulPolledTimeStamp = 0;
        m_smaEnableLock.release();
    }
}

void PasPollingWorker::notifyObservers()
{
    TA_Base_Bus::DataBlock<unsigned short> dbPollingDataCopy( m_dbPolledData );

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxObserversLock );

    for ( std::list<IPasPollingObserver*>::iterator itLoop = m_lstPollingObservers.begin();
            m_lstPollingObservers.end() != itLoop; ++itLoop )
    {
        IPasPollingObserver* pPollingObserver = *itLoop;
        if ( NULL != pPollingObserver )
        {
            pPollingObserver->processPollingUpdate( dbPollingDataCopy, m_ulPolledTimeStamp );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Invalid parameter [NULL pointer] while calling PasPollingWorker::notifyObservers" );
        }
    }
}

bool PasPollingWorker::isInPollingRange( const int nStartAddress, const int nEndAddress )
{
    if (( nStartAddress >= m_dbPolledData.start() ) && ( nEndAddress <= m_dbPolledData.end() ))
    {
        return true;
    }

    return false;
}

void PasPollingWorker::registerPollingObserver( IPasPollingObserver& refObserver )
{
    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxObserversLock );

    if ( m_lstPollingObservers.end() == std::find( m_lstPollingObservers.begin(), m_lstPollingObservers.end(), &refObserver ))
    {
        m_lstPollingObservers.push_back( &refObserver );
    }
}

void PasPollingWorker::deregisterPollingObserver( IPasPollingObserver& refObserver )
{
    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxObserversLock );

    std::list<IPasPollingObserver*>::iterator itFound = std::find( m_lstPollingObservers.begin(), m_lstPollingObservers.end(), &refObserver );

    if ( m_lstPollingObservers.end() != itFound )
    {
        m_lstPollingObservers.erase( itFound );
    }
}

void PasPollingWorker::initPollingSetting( const int nStartAddress, const int nEndAddress, const long lPollingInterval )
{
    if (( 0 > nStartAddress ) || ( nEndAddress < nStartAddress ) || ( PAS_POLLING_MIN_INTERVAL > lPollingInterval ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Invalid input parameters [%d, %d, %d] while calling PasPollingWorker::initPollingSetting", 
            nStartAddress, nEndAddress, lPollingInterval );
        return;
    }

    m_dbPolledData.setRange( nStartAddress, nEndAddress );
    m_lPollingIntervalMilSec = lPollingInterval;
    m_bInited = true;
}

}
