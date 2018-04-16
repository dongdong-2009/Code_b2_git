/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasPollingManager.h $
 * @author  HuangQi
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef PASPOLLINGMANAGER_H
#define PASPOLLINGMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "ace/RW_Mutex.h"

#include "app/pa/PAAgent/src/IPasConnectionObserver.h"
#include "app/pa/PAAgent/src/PasPollingWorker.h"


namespace TA_IRS_App
{

class IPasPollingObserver;

class PasPollingManager : IPasConnectionObserver
{
public:
    PasPollingManager( PasConnectionAgency& refPasConnection );
    virtual ~PasPollingManager();

    virtual void connectionUpdated( bool bConnectionAvailable );

    void registerPollingObserver( IPasPollingObserver& refObserver, const int nStartAddress, const int nEndAddress );
    void deregisterPollingObserver( IPasPollingObserver& refObserver, const int nStartAddress, const int nEndAddress );

    void enable( bool bEnable = true );

private:
    PasPollingManager();
	PasPollingManager( const PasPollingManager& );
    const PasPollingManager& operator=( const PasPollingManager& );

    void init();
    void clean();
    void createPollingTask( const int nStartAddress, const int nEndAddress, long lPollingInterval );

protected:
    PasConnectionAgency& m_refPasConnection;

    std::list<PasPollingWorker> m_lstPasPollingWorkers;

    mutable ACE_RW_Mutex m_mtxPollingWorkerLock;

    bool m_bEnable;
};

} // namespace TA_IRS_App

#endif // PASPOLLINGMANAGER_H
