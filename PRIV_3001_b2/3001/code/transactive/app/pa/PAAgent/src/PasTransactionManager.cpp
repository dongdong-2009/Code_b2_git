/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasTransactionManager.cpp $
  * @author:  HuangQi
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

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasTransaction.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"

namespace TA_IRS_App
{

const int TRANSACTION_PROCESS_SLEEP_TIMEOUT = 30;
const int DEFAULT_TASK_INDEX = 0;

PasTransactionManager* PasTransactionManager::s_pInstance = NULL;

PasTransactionManager::PasTransactionManager( PasConnectionAgency& refPasConnection ) :
m_refPasConnection( refPasConnection ),
m_bRunning( false ),
m_bEnable( false ),
m_bConnectionAvailable( false ),
m_lstTransactions(),
m_pCurExecutingTask( NULL ),
m_mapTransactionWorkers(),
m_mtxTransactionListLock(),
m_mtxTransactionExecutionLock(),
m_mtxTransactionWorkersLock(),
m_smaTransactionCount( 0 ),
m_smaControlMode( 0 )
{
    m_refPasConnection.registerConnectionStatusObserver( this );
}

PasTransactionManager::~PasTransactionManager()
{
    m_refPasConnection.deregisterConnectionStatusObserver( this );

    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }

    clearTransactions();
}

PasTransactionManager& PasTransactionManager::getInstance()
{
    if ( NULL == s_pInstance )
    {
        TA_THROW( TA_Base_Core::TransactiveException("Instance of PasTransactionManager is not created") );
    }

    return *s_pInstance;
}

void PasTransactionManager::createInstance( PasConnectionAgency& refPasConnection )
{
    if ( NULL != s_pInstance )
    {
        return;
    }

    s_pInstance = new PasTransactionManager( refPasConnection );
}

void PasTransactionManager::destoryInstance()
{
    if ( NULL == s_pInstance )
    {
        return;
    }

    delete s_pInstance;
    s_pInstance = NULL;
}

void PasTransactionManager::enable( bool bEnable )
{
    if ( m_bEnable == bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    if ( bEnable )
    {
        m_smaControlMode.release();
    }
    else
    {
        clearTransactions();
    }

    // Enable/disable transaction worker
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxTransactionWorkersLock );

        for ( TransactionWorkerMapIt itLoop = m_mapTransactionWorkers.begin(); 
            m_mapTransactionWorkers.end() != itLoop; ++ itLoop )
        {
            for ( IndexedTaskMapIt itTaskLoop = itLoop->second.begin(); 
                itLoop->second.end() != itTaskLoop; ++itTaskLoop )
            {
                itTaskLoop->second.enable( bEnable );
            }
        }
    }
}

void PasTransactionManager::connectionUpdated( bool bConnectionAvailable )
{
    m_bConnectionAvailable = bConnectionAvailable;
}

void PasTransactionManager::run()
{
    m_bRunning = true;
    ACE_Time_Value atmSleep = ACE_Time_Value::zero;
    atmSleep.msec( TRANSACTION_PROCESS_SLEEP_TIMEOUT );

    while ( m_bRunning )
    {
        try
        {
            if ( !m_bEnable )
            {
                m_smaControlMode.acquire();
            }

            m_smaTransactionCount.acquire();

            if ( !m_bRunning )
            {
                break;
            }

            processTransaction();

            ACE_OS::sleep( atmSleep );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "Caught unknown exception when PasTransactionManager running" );
        }
    }
}

void PasTransactionManager::terminate()
{
    m_bRunning = false;
    m_smaTransactionCount.release();
    m_smaControlMode.release();
}

int PasTransactionManager::addTransaction( PasTransaction* pTask )
{
    if ( !m_bEnable )
    {
        delete pTask;
        pTask = NULL;
        return TRANSACTION_IN_MONITOR_MODE;
    }

    if ( !m_bConnectionAvailable )
    {
        delete pTask;
        pTask = NULL;
        return TRANSACTION_CONNECTION_FAILURE;
    }

    if ( NULL == pTask )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Invalid input parameter [NULL pointer] while calling PasTransactionManager::addTransaction" );
        return INVALID_NULL_TRANSACTION;
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );
        m_lstTransactions.push_back( pTask );

        m_smaTransactionCount.release();
    }

    return TRANSACTION_QUEUED;
}

int PasTransactionManager::executeTransaction( PasTransaction& refTask, const bool bEnableConcurrent  )
{
    if ( !m_bEnable )
    {
        return TRANSACTION_IN_MONITOR_MODE;
    }

    if ( !m_bConnectionAvailable )
    {
        return TRANSACTION_CONNECTION_FAILURE;
    }

    int nResult = TRANSACTION_NOT_EXECUTED;

    if ( bEnableConcurrent )
    {
        nResult = refTask.execute( m_refPasConnection );
    }
    else
    {
        ACE_Guard<ACE_RW_Mutex> guard( m_mtxTransactionExecutionLock );
        nResult = refTask.execute( m_refPasConnection );
    }

    return nResult;
}

void PasTransactionManager::processTransaction()
{
    ACE_Guard<ACE_RW_Mutex> guard( m_mtxTransactionExecutionLock );

    m_pCurExecutingTask = NULL;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );
        if ( 0 == m_lstTransactions.size() )
        {
            return;
        }

        m_pCurExecutingTask = m_lstTransactions.front();
    }

    if ( NULL == m_pCurExecutingTask )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "NULL pointer in transaction list" );
        return;
    }

    try
    {
        m_pCurExecutingTask->execute( m_refPasConnection );
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when calling PasTransaction::execute" );
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );

        if ( m_pCurExecutingTask == m_lstTransactions.front() )
        {
            m_lstTransactions.pop_front();

            PasTransaction* pToDeleted = m_pCurExecutingTask;
            m_pCurExecutingTask = NULL;

            delete pToDeleted;
            pToDeleted = NULL;
        }
    }
}

void PasTransactionManager::abandonTransaction( ITransactionOwner& refOnwer )
{
    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );
    for ( PasTransactionListIt itLoop = m_lstTransactions.begin(); itLoop != m_lstTransactions.end(); ++itLoop )
    {
        PasTransaction* pTask = *itLoop;
        if ( NULL == pTask )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Invalid input parameter [NULL pointer] while calling PasTransactionManager::abandonTransaction" );
            continue;
        }

        pTask->cancelOwnerIfRelated( refOnwer );
    }
}

bool PasTransactionManager::removeTransaction( ITransactionOwner& refOnwer )
{
    bool bRemoved = false;

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );
    for ( PasTransactionListIt itLoop = m_lstTransactions.begin(); itLoop != m_lstTransactions.end(); ++itLoop )
    {
        PasTransaction* pTask = *itLoop;
        if ( NULL == pTask )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Invalid input parameter [NULL pointer] while calling PasTransactionManager::abandonTransaction" );
            continue;
        }

        // Lock for the executing task, if executing, skip it
        if ( m_pCurExecutingTask == pTask )
        {
            continue;
        }

        // Not executing, can remove.
        if ( pTask->cancelOwnerIfRelated( refOnwer ))
        {
            m_lstTransactions.erase( itLoop );

            delete pTask;
            pTask = NULL;

            bRemoved = true;
            break;
        }
    }

    return bRemoved;
}

void PasTransactionManager::clearTransactions()
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionListLock );

        for ( PasTransactionListIt itLoop = m_lstTransactions.begin(); m_lstTransactions.end() != itLoop; ++itLoop )
        {
            delete *itLoop;
            *itLoop = NULL;
        }

        m_lstTransactions.clear();
    }
}

void PasTransactionManager::signalTransaction( ITransactionWorkerUser& refWorkerUser )
{
    signalTransaction( refWorkerUser, DEFAULT_TASK_INDEX );
}

void PasTransactionManager::signalTransaction( ITransactionWorkerUser& refWorkerUser, int nTaskIndex )
{
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxTransactionWorkersLock );

        TransactionWorkerMapIt itFound = m_mapTransactionWorkers.find( &refWorkerUser );

        if ( m_mapTransactionWorkers.end() == itFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transaction worker user is not binded yet" );
            return;
        }

        IndexedTaskMapIt itWorker = itFound->second.find( nTaskIndex );

        if ( itFound->second.end() == itWorker )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transaction worker user is not binded yet" );
            return;
        }

        itWorker->second.signalTransaction();
    }
}

void PasTransactionManager::keepWorkTransaction( ITransactionWorkerUser& refWorkerUser, const int nTaskIndex, const bool bKeepWorking )
{
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxTransactionWorkersLock );

        TransactionWorkerMapIt itFound = m_mapTransactionWorkers.find( &refWorkerUser );

        if ( m_mapTransactionWorkers.end() == itFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transaction worker user is not binded yet" );
            return;
        }

        IndexedTaskMapIt itWorker = itFound->second.find( nTaskIndex );

        if ( itFound->second.end() == itWorker )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transaction worker user is not binded yet" );
            return;
        }

        itWorker->second.keepWorking( bKeepWorking );
    }
}

void PasTransactionManager::bindTransactionWorker( ITransactionWorkerUser& refWorkerUser, PasTransaction* pTask, 
    const int nTaskIndex, const bool bTransactionReadonly, const long lRetryInterval, const long lWorkingInterval )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionWorkersLock );

        TransactionWorkerMapIt itFound = m_mapTransactionWorkers.find( &refWorkerUser );

        if ( m_mapTransactionWorkers.end() == itFound )
        {
            IndexedTaskMap mapNewTask;
            std::pair<TransactionWorkerMapIt, bool> pirResult = m_mapTransactionWorkers.insert( 
                TransactionWorkerMap::value_type( &refWorkerUser, mapNewTask ));

            itFound = pirResult.first;

            // Check insertion result
            if (( m_mapTransactionWorkers.end() == itFound ) || !pirResult.second )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert new IndexedTaskMap" );
                return;
            }
        }

        PasTransactionWorker oTempWorker( *this );
        std::pair<IndexedTaskMapIt, bool> pirTaskResult = itFound->second.insert( 
            IndexedTaskMap::value_type( nTaskIndex, oTempWorker ));

        if ( !pirTaskResult.second )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert new PasTransactionWorker" );
            return;
        }

        pirTaskResult.first->second.initTransaction( pTask, bTransactionReadonly, lRetryInterval, lWorkingInterval );
        pirTaskResult.first->second.start();
    }
}

void PasTransactionManager::bindTransactionWorker( ITransactionWorkerUser& refWorkerUser, PasTransaction* pTask, const bool bTransactionReadonly )
{
    bindTransactionWorker( refWorkerUser, pTask, DEFAULT_TASK_INDEX, bTransactionReadonly );
}

void PasTransactionManager::unbindTransactionWorker( ITransactionWorkerUser& refWorkerUser )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxTransactionWorkersLock );

        TransactionWorkerMapIt itFound = m_mapTransactionWorkers.find( &refWorkerUser );

        if ( m_mapTransactionWorkers.end() == itFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transaction worker user is not binded yet" );
            return;
        }

        for ( IndexedTaskMapIt itTaskLoop = itFound->second.begin(); 
            itFound->second.end() != itTaskLoop; ++itTaskLoop )
        {
            itTaskLoop->second.terminateAndWait();
        }

        itFound->second.clear();
        m_mapTransactionWorkers.erase( itFound );
    }
}

bool PasTransactionManager::isConnectionAvailable()
{
    return m_bConnectionAvailable;
}

}
