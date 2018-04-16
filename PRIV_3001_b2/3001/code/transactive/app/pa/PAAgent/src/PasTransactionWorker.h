/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasTransactionWorker.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef PASTRANSACTIONWORKER_H
#define PASTRANSACTIONWORKER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "ace/Thread_Semaphore.h"
#include "ace/Mutex.h"

#include "core/threads/src/Thread.h"

namespace TA_IRS_App
{

class PasTransaction;
class ITransactionExecutor;

const long TRANSACTION_RETRY_INTERVAL = 1000;
const long DEFAULT_KEEP_WORKING_INTERVAL = 100;

class PasTransactionWorker : public TA_Base_Core::Thread
{
public:
    PasTransactionWorker( ITransactionExecutor& refExecutor );
    PasTransactionWorker( const PasTransactionWorker& refSrc );
    virtual ~PasTransactionWorker();

    void initTransaction( PasTransaction* pTransaction, const bool bEnableConcurrent, 
        const long lRetryInterval, const long lWorkingInterval );

    void run();
    void terminate();

    void enable( const bool bEnable = true );

    void signalTransaction();
    void keepWorking( const bool bKeepWorking = true );

private:
    PasTransactionWorker();
	const PasTransactionWorker& operator=( const PasTransactionWorker& );

protected:
    void workTillSuccess();
    void workOnce();

protected:
    volatile bool m_bRunning;
    volatile bool m_bEnable;
    bool m_bEnableConcurrent;
    bool m_bInited;
    long m_lRetryIntervalMilSec;
    long m_lKeepWorkIntervalMilSec;
    int m_nRequireKeepWorking;

    PasTransaction* m_pTransaction;
    ITransactionExecutor& m_refExecutor;

    ACE_Mutex m_mtxKeepWorkingFlag;
    ACE_Thread_Semaphore m_smaEnableLock;
    ACE_Thread_Semaphore m_smaWorkLock;
};

} // namespace TA_IRS_App

#endif // PASTRANSACTIONWORKER_H
