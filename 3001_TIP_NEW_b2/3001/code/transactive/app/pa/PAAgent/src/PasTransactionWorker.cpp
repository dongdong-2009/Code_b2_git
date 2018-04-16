/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasTransactionWorker.cpp $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/25 14:06:39 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasTransaction.h"
#include "app/pa/PAAgent/src/ITransactionExecutor.h"
#include "app/pa/PAAgent/src/PasTransactionWorker.h"

namespace TA_IRS_App
{

PasTransactionWorker::PasTransactionWorker( ITransactionExecutor& refExecutor ) : 
TA_Base_Core::Thread(),
m_bRunning( true ),
m_bEnable( false ),
m_nRequireKeepWorking( 0 ),
m_bEnableConcurrent( false ),
m_bInited( false ),
m_lRetryIntervalMilSec( TRANSACTION_RETRY_INTERVAL ),
m_lKeepWorkIntervalMilSec( DEFAULT_KEEP_WORKING_INTERVAL ),
m_pTransaction( NULL ),
m_refExecutor( refExecutor ),
m_smaEnableLock( 0 ),
m_smaWorkLock( 0 )
{
}

PasTransactionWorker::PasTransactionWorker( const PasTransactionWorker& refSrc ) : 
TA_Base_Core::Thread(),
m_bRunning( refSrc.m_bRunning ),
m_bEnable( refSrc.m_bEnable ),
m_nRequireKeepWorking( refSrc.m_nRequireKeepWorking ),
m_bEnableConcurrent( refSrc.m_bEnableConcurrent ),
m_bInited( refSrc.m_bInited ),
m_lRetryIntervalMilSec( refSrc.m_lRetryIntervalMilSec ),
m_lKeepWorkIntervalMilSec( refSrc.m_lKeepWorkIntervalMilSec ),
m_pTransaction( refSrc.m_pTransaction ),
m_refExecutor( refSrc.m_refExecutor ),
m_smaEnableLock( 0 ),
m_smaWorkLock( 0 )
{
}

PasTransactionWorker::~PasTransactionWorker()
{
    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }

    if ( NULL != m_pTransaction )
    {
        delete m_pTransaction;
        m_pTransaction = NULL;
    }
}

void PasTransactionWorker::run()
{
    if ( !m_bInited )
    {
        return;
    }

    m_bRunning = true;

    ACE_Time_Value atmSleep = ACE_Time_Value::zero;
    atmSleep.msec( m_lKeepWorkIntervalMilSec );

    while ( m_bRunning )
    {
        if ( !m_bEnable )
        {
            m_smaEnableLock.acquire();
        }

        // If no body require keep working, just work once
        if ( 0 == m_nRequireKeepWorking )
        {
            m_smaWorkLock.acquire();
        }

        if ( !m_bRunning )
        {
            break;
        }

        if ( 0 < m_nRequireKeepWorking )
        {
            workOnce();

            ACE_OS::sleep( atmSleep );
        }
        else
        {
            workTillSuccess();
        }
    }
}

void PasTransactionWorker::terminate()
{
    m_bRunning = false;
    m_smaWorkLock.release();
    m_smaEnableLock.release();
}

void PasTransactionWorker::enable( const bool bEnable )
{
    if ( m_bEnable == bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    if ( bEnable )
    {
        m_smaEnableLock.release();
    }
}

void PasTransactionWorker::workTillSuccess()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Transaction execution started" );

    int nResult = TRANSACTION_NOT_EXECUTED;

    ACE_Time_Value atmSleep = ACE_Time_Value::zero;
    atmSleep.msec( m_lRetryIntervalMilSec );

    while ( TRANSACTION_SUCCESS != nResult )
    {
        if ( !m_bRunning || !m_bEnable )
        {
            break;
        }

        nResult = m_refExecutor.executeTransaction( *m_pTransaction, m_bEnableConcurrent );

        if ( TRANSACTION_SUCCESS != nResult )
        {
            ACE_OS::sleep( atmSleep );
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Transaction execution ended" );
}

void PasTransactionWorker::workOnce()
{
    int nResult = m_refExecutor.executeTransaction( *m_pTransaction, m_bEnableConcurrent );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Transaction execution ended with result %d", nResult );
}

void PasTransactionWorker::signalTransaction()
{
    m_smaWorkLock.release();
}

void PasTransactionWorker::keepWorking( const bool bKeepWorking )
{
    // Function lock
    ACE_Guard<ACE_Mutex> guard(m_mtxKeepWorkingFlag);

    if ( bKeepWorking )
    {
        ++m_nRequireKeepWorking;
        m_smaWorkLock.release();
    }
    else
    {
        --m_nRequireKeepWorking;
        m_smaWorkLock.tryacquire();
    }
}

void PasTransactionWorker::initTransaction( PasTransaction* pTransaction, const bool bEnableConcurrent,
    const long lRetryInterval, const long lWorkingIntreval )
{
    if ( NULL == pTransaction )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Invalid input parameter [NULL pointer] while calling PasTransactionWorker::initTransaction" );
        return;
    }

    m_pTransaction = pTransaction;
    m_bEnableConcurrent = bEnableConcurrent;
    m_lRetryIntervalMilSec = lRetryInterval;
    m_lKeepWorkIntervalMilSec = lWorkingIntreval;

    m_bInited = true;
}

}
