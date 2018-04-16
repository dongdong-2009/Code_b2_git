/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/EMSAgent/src/PEnergyTimerWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The PEnergyTimerWorker object is responsible for executing the task timely
  *
  */

#ifndef TIMER_WORKER_H
#define TIMER_WORKER_H

#include <string>
#include <vector>

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"

namespace TA_Base_App
{
	class PEnergyTimerWorker : public TA_Base_Core::Thread,
						public TA_Base_Core::ITimeoutCallback
	{
	// operations
	public:

		PEnergyTimerWorker(int TimeOut, int startDelayTime);
                          
        virtual ~PEnergyTimerWorker();

        /**
		  * setInServiceState
		  *
		  * Set the worker thread in or out of service.  A worker thread that is out of service
		  * would close down the ModbusTcpConnection and not perform any task
		  *
		  * @param inService	Flag to indicate if the worker thread is in service or not
		  *
		  */
		virtual void setInServiceState ( bool inService );

		virtual bool isInService() const;
		
        virtual bool isThreadRunning() const;
		
	// operations
	private:
        // disabled constructors
		PEnergyTimerWorker();
		PEnergyTimerWorker ( const PEnergyTimerWorker & obj );
		PEnergyTimerWorker & operator= ( const PEnergyTimerWorker & rhs );

		//
		// operations required by the TA_Core::TA_Thread interface
		//
		virtual void run();
		virtual void terminate();
		virtual void process();

		//
		// operations specific to ITimeoutCallback interface
		//

		/**
		  *
          * timerExpired
          *
          * This operation will be called when the registered timeout period has expired.
          *
          */
		virtual void timerExpired(long timerId, void* userData);
	protected:
        int m_timeOut;

		volatile bool m_inService;
        volatile bool m_threadTerminated;    
	// attributes
	private:
        TA_Base_Core::SingletonTimerUtil&   m_timerUtility;
  		TA_Base_Core::Semaphore				m_timerSemaphore;
		int									m_startDelayTime;
	};
}

#endif
