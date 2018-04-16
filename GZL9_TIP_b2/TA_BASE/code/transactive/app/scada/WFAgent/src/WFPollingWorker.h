#ifndef WFPOLLINGWORKER_H
#define WFPOLLINGWORKER_H

#include <vector>
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	// Forward declaration
	class WfUnit;

	class WfPollingWorker:	public TA_Base_Core::Thread,
							public TA_Base_Core::ITimeoutCallback
	{
	public:
		
		WfPollingWorker();
		
		virtual ~WfPollingWorker();


		void addWfUnit(WfUnit& wfUnit);


		void updatePollingPeriod(int newPeriodInSec);
		
		/**
		* setInServiceState
		* Set the worker thread in or out of service.  A worker thread that is out of service
		* would close down the ModbusTcpConnection and not perform any task
		* @param inService	Flag to indicate if the worker thread is in service or not
		*/
		virtual void setInServiceState ( bool inService );

		//
		//  TA_Core::TA_Thread interfaces
		//
		virtual void run();

		virtual void terminate();


		/* 
		* This operation will be called when the registered timeout period has expired.
		*/
		virtual void timerExpired(long timerId, void* userData);


		// disabled constructors
		WfPollingWorker ( const WfPollingWorker & obj );
		WfPollingWorker & operator= ( const WfPollingWorker & rhs );
	private:
		TA_Base_Core::SingletonTimerUtil&		       m_timerUtility;
		TA_Base_Core::Condition					       m_timerCondition;
		std::vector<WfUnit*>	    m_wfUnits;
		int							m_periodInSec;
		volatile bool				m_inService;
        volatile bool				m_threadTerminated;
		volatile bool				m_periodInSecUpdated;
		volatile bool				m_actived;
	};
}

#endif

