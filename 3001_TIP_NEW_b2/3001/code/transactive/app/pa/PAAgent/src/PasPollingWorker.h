/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasPollingWorker.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef PASPOLLINGWORKER_H
#define PASPOLLINGWORKER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "ace/Thread_Semaphore.h"
#include "ace/RW_Mutex.h"

#include "core/threads/src/Thread.h"
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_IRS_App
{

class PasConnectionAgency;
class IPasPollingObserver;

class PasPollingWorker : public TA_Base_Core::Thread
{
public:
    PasPollingWorker( PasConnectionAgency& refPasConnection );
    PasPollingWorker( const PasPollingWorker& refSrc );
    virtual ~PasPollingWorker();

    void run();
    void terminate();

    void initPollingSetting( const int nStartAddress, const int nEndAddress, const long lPollingInterval );
    void enable( const bool bEnable = true );
    bool isInPollingRange( const int nStartAddress, const int nEndAddress );

    void registerPollingObserver( IPasPollingObserver& refObserver );
    void deregisterPollingObserver( IPasPollingObserver& refObserver );

private:
    PasPollingWorker();
	const PasPollingWorker& operator=( const PasPollingWorker& );

protected:
    bool polling();
    void notifyObservers();

protected:
    PasConnectionAgency& m_refPasConnectionAgentcy;

    volatile bool m_bRunning;
    volatile bool m_bEnable;
    bool m_bInited;
    TA_Base_Bus::DataBlock<unsigned short> m_dbPolledData;
    unsigned long m_ulPolledTimeStamp;
    long m_lPollingIntervalMilSec;

    std::list<IPasPollingObserver*> m_lstPollingObservers;

    ACE_Thread_Semaphore m_smaEnableLock;
    mutable ACE_RW_Mutex m_mtxObserversLock;
};

} // namespace TA_IRS_App

#endif // PasPollingWorker_H
