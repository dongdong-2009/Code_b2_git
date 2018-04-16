/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasTransactionManager.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef PASTRANSACTIONMANAGER_H
#define PASTRANSACTIONMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "ace/RW_Mutex.h"
#include "ace/Thread_Semaphore.h"

#include "core/threads/src/Thread.h"
#include "app/pa/PAAgent/src/IPasConnectionObserver.h"
#include "app/pa/PAAgent/src/ITransactionExecutor.h"
#include "app/pa/PAAgent/src/PasTransactionWorker.h"

namespace TA_IRS_App
{

class PasConnectionAgency;
class PasTransaction;
class ITransactionOwner;
class ITransactionWorkerUser;

typedef std::list<PasTransaction*> PasTransactionList;
typedef std::list<PasTransaction*>::iterator PasTransactionListIt;

typedef std::map<int, PasTransactionWorker> IndexedTaskMap;
typedef std::map<int, PasTransactionWorker>::iterator IndexedTaskMapIt;
typedef std::map<ITransactionWorkerUser*, IndexedTaskMap> TransactionWorkerMap;
typedef std::map<ITransactionWorkerUser*, IndexedTaskMap>::iterator TransactionWorkerMapIt;

class PasTransactionManager : public TA_Base_Core::Thread, public IPasConnectionObserver, public ITransactionExecutor
{
public:
    virtual ~PasTransactionManager();
    static void createInstance( PasConnectionAgency& refPasConnection );
    static void destoryInstance();
    static PasTransactionManager& getInstance();

    virtual void connectionUpdated( bool bConnectionAvailable );

    virtual void run();
    virtual void terminate();

    virtual int executeTransaction( PasTransaction& refTask, const bool bEnableConcurrent );

    void enable( bool bEnable = true );

    // Pointer owner transfer to PasTransactionManager
    int addTransaction( PasTransaction* pTask );
    void abandonTransaction( ITransactionOwner& refOnwer );
    bool removeTransaction( ITransactionOwner& refOnwer );

    bool isConnectionAvailable();

    void bindTransactionWorker( ITransactionWorkerUser& refWorkerUser, PasTransaction* pTask, const int nTaskIndex, const bool bTransactionReadonly, 
        const long lRetryInterval = TRANSACTION_RETRY_INTERVAL, const long lWorkingInterval = DEFAULT_KEEP_WORKING_INTERVAL );
    void bindTransactionWorker( ITransactionWorkerUser& refWorkerUser, PasTransaction* pTask, const bool bTransactionReadonly );
    void unbindTransactionWorker( ITransactionWorkerUser& refWorkerUser );
    void signalTransaction( ITransactionWorkerUser& refWorkerUser, const int nTaskIndex );
    void signalTransaction( ITransactionWorkerUser& refWorkerUser );
    void keepWorkTransaction( ITransactionWorkerUser& refWorkerUser, const int nTaskIndex, const bool bKeepWorking = true );

private:
    PasTransactionManager();
    PasTransactionManager( PasConnectionAgency& refPasConnection );
	PasTransactionManager( const PasTransactionManager& );
	const PasTransactionManager& operator=( const PasTransactionManager& );

    void processTransaction();
    void clearTransactions();

private:
    static PasTransactionManager* s_pInstance;
    PasConnectionAgency& m_refPasConnection;
    volatile bool m_bRunning;
    bool m_bEnable;
    bool m_bConnectionAvailable;

    PasTransactionList m_lstTransactions;
    PasTransaction* m_pCurExecutingTask;
    TransactionWorkerMap m_mapTransactionWorkers;

    mutable ACE_RW_Mutex m_mtxTransactionListLock;
    mutable ACE_RW_Mutex m_mtxTransactionExecutionLock;
    mutable ACE_RW_Mutex m_mtxTransactionWorkersLock;
    ACE_Thread_Semaphore m_smaTransactionCount;
    ACE_Thread_Semaphore m_smaControlMode;
};

} // namespace TA_IRS_App

#endif // PASTRANSACTIONMANAGER_H
