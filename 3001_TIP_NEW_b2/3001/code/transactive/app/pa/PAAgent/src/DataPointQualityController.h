/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/DataPointQualityController.h $
 * @author  J. Dalin
 * @version $Revision: #3 $
 * Last modification : $DateTime: 2012/03/26 10:45:17 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef DATAPOINTQUALITYCONTROLLER_H
#define DATAPOINTQUALITYCONTROLLER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <list>

#include "ace/Thread_Semaphore.h"
#include "ace/RW_Mutex.h"

#include "core/threads/src/Thread.h"
#include "app/pa/PAAgent/src/IPasConnectionObserver.h"

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{

class PasConnectionAgency;
class IPasPollingObserver;

class DataPointQualityController : public TA_Base_Core::Thread, public virtual IPasConnectionObserver
{
public:
    DataPointQualityController( ACE_RW_Mutex& refDataPointsListLock, std::vector<TA_Base_Bus::DataPoint*>& refDataPoints );
    virtual ~DataPointQualityController();

    void run();
    void terminate();
    void enable( bool bEnable = true );

    virtual void connectionUpdated( bool bConnectionAvailable );

private:
    DataPointQualityController();
	const DataPointQualityController& operator=( const DataPointQualityController& );

protected:
    void processLatestUpdate();

protected:
    ACE_RW_Mutex& m_refDataPointsListLock;
    std::vector<TA_Base_Bus::DataPoint*>& m_refDataPoints;

    ACE_RW_Mutex m_mtxUpdatesLock;
    std::list<bool> m_lstQualityUpdates;

    volatile bool m_bRunning;
    volatile bool m_bLatestQualityState;
    volatile bool m_bForceUpdate;

    ACE_Thread_Semaphore m_smaUpdateRequest;
};

} // namespace TA_IRS_App

#endif // DATAPOINTQUALITYCONTROLLER_H
