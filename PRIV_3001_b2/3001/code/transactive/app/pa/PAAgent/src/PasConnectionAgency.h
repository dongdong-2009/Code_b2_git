/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasConnectionAgency.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef PASCONNECTIONAGENCY_H
#define PASCONNECTIONAGENCY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "ace/Thread_Semaphore.h"
#include "ace/RW_Mutex.h"

#include "core/threads/src/Thread.h"
#include "app/pa/PAAgent/src/PasModbusConnection.h"

namespace TA_Base_Bus
{
    template<class DataType> class DataBlock;
}

namespace TA_IRS_App
{

class PasModbusConnection;
class IPasConnectionObserver;

const int MAX_COMMUNICATION_RETRY_TIMES = 2;

class PasConnectionAgency : public TA_Base_Core::Thread
{
public:
    PasConnectionAgency();
    virtual ~PasConnectionAgency();

    virtual void run();
    virtual void terminate();

    void enable( const bool bEnable = true );

    bool writeToPasWithRetry( const TA_Base_Bus::DataBlock<unsigned short>& refData, 
        const int nTryTimes = MAX_COMMUNICATION_RETRY_TIMES );
    bool readFromPasWithRetry( TA_Base_Bus::DataBlock<unsigned short>& refData, 
        const int nTryTimes = MAX_COMMUNICATION_RETRY_TIMES );

    void registerConnectionStatusObserver( IPasConnectionObserver* pObserver );
    void deregisterConnectionStatusObserver( IPasConnectionObserver* pObserver );
    void clearObservers();

private:
	PasConnectionAgency( const PasConnectionAgency& );
	const PasConnectionAgency& operator=( const PasConnectionAgency& );

    bool checkConnections( bool bAdvanceStrategy );
    bool updateCurAvailablePasConnection();
    void notifyConnectionObservers();

    bool writeToPas( const TA_Base_Bus::DataBlock<unsigned short>& refData );
    bool readFromPas( TA_Base_Bus::DataBlock<unsigned short>& refData );

    volatile bool m_bRunning;
    volatile bool m_bEnable;
    volatile bool m_bForceNotifyObservers;
    PasModbusConnection m_oPrimaryPasConnection;
    PasModbusConnection m_oSecondaryPasConnection;
    PasModbusConnection* m_pCurAvailablePasConnection;

    std::list<IPasConnectionObserver*> m_lstPasConnObservers;

    mutable ACE_RW_Mutex m_mtxPasConnObservsers;
    mutable ACE_RW_Mutex m_mtxPasConnReadWrite;
    mutable ACE_RW_Mutex m_mtxCurAvailablePasConn;

    ACE_Thread_Semaphore m_smaControlMode;
};

} // namespace TA_IRS_App

#endif // PASCONNECTIONAGENCY_H
