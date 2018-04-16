/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PMSAgent/src/RTUPollingWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The PEnergyTimerWorker object is running timely.
  *
  */

#include <sys/timeb.h>

#include "PEnergyTimerWorker.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_App
{
	
	PEnergyTimerWorker::PEnergyTimerWorker( int TimeOut, int startDelayTime)
	:
    m_timeOut( TimeOut*1000 ),
    m_inService( false ),
    m_threadTerminated( false ),
    m_timerSemaphore( 0 ),
	m_timerUtility( TA_Base_Core::SingletonTimerUtil::getInstance()),
	m_startDelayTime(startDelayTime*1000)
	{
 	}


	PEnergyTimerWorker::~PEnergyTimerWorker()
	{
        //TD13398, should stop this thread first, then stop the timers.
		// Ensure that PEnergyTimerWorker thread is stopped
        setInServiceState( false );

		// Stop the timers
		m_timerUtility.stopPeriodicTimeOutClock(this);
		TA_Base_Core::SingletonTimerUtil::removeInstance();

	}


	void PEnergyTimerWorker::setInServiceState( bool inService )
	{
        // prevent setting the same state
        if (m_inService == inService)
        {
            return;
        }

		m_inService = inService;

		if ( true == inService )
		{
			start();
		}
		else
		{
            terminateAndWait();
		}
	}

	bool PEnergyTimerWorker::isInService() const
	{
		return m_inService;
	}


	void PEnergyTimerWorker::terminate()
	{
		// terminate polling thread
		m_threadTerminated = true;

        // signalling wont do any good if its not waiting
        // worst case is wait for the timer
        // it will terminate on the next poll cycle
		m_timerSemaphore.post();
	}


	void PEnergyTimerWorker::run()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Thread start to wait for %d millisecond",m_startDelayTime);
		TA_Base_Core::Thread::sleep( m_startDelayTime );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Thread stop to wait");
	
		m_timerUtility.startPeriodicTimeOutClock( this, m_timeOut, false );

		while ( false == m_threadTerminated )
		{
			// if the worker thread is in service
			if ( true == m_inService )
			{
				process();
            }   

			m_timerSemaphore.wait();
		}

		m_timerUtility.stopPeriodicTimeOutClock(this);

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}

	void PEnergyTimerWorker::process()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Loop test for PEnergyTimerWorker");
	}
	

	void PEnergyTimerWorker::timerExpired(long timerId, void* userData)
	{
		m_timerSemaphore.post();
	}

	bool PEnergyTimerWorker::isThreadRunning() const
	{
		return ( THREAD_STATE_RUNNING == getCurrentState() );
	}
}





















