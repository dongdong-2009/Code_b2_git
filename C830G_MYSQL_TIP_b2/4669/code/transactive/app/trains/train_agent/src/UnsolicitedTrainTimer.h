#if !defined(UNSOLICITEDTRAINTIMER_H)
#define UNSOLICITEDTRAINTIMER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/UnsolicitedTrainTimer.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This is used to start timers that notify when they expire by a callback.
  * TrainTimer must be waited on to catch timeout events.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <map>
#include <string>

#include "ace/ACE.h"
#include "ace/Thread_Manager.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue.h"
#include "ace/Timer_Queue_Adapters.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

namespace TA_IRS_App
{
    // implemented by classes that use UnsolicitedTrainTimer
    class IUnsolicitedTimerCallback
    {
    public:
        virtual void handleTimeout(unsigned char trainId) = 0;
    };


	class UnsolicitedTrainTimer : public ACE_Event_Handler
	{

	public:

		UnsolicitedTrainTimer(IUnsolicitedTimerCallback& callback);
		~UnsolicitedTrainTimer();

        /** 
          * startTimer
          *
          * Starts (or resets) the timer for a train and provide an optional description for the timer
          *
          * @param trainId
          * @param timeoutMillis
          * @param description An arbitrary string that describes this timer
          */
		void startTimer(unsigned int trainId, unsigned int timeoutMillis, bool logTimerOn = false, std::string description = "");

        /** 
          * endTimer
          *
          * Ends a timer for a train
          *
          * @param trainId
          */
        void endTimer(unsigned int trainId);


	protected:

		int handle_timeout ( const ACE_Time_Value &tv, const void *constArg );

	protected:
		typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveQueue;

        /**
	* Storage for associating a timer with a train
	*/
        struct TimerItem
        {
            unsigned char trainId;
            long timerId;
			std::string description; // Arbitrary description of this timer
			bool logTimerOn; // flag to indicate if this timer should be logged
        };
		
		ActiveQueue									m_activeQueue;
		IUnsolicitedTimerCallback&					m_callback;

		TA_Base_Core::ReEntrantThreadLockable		m_mapLock;
        typedef std::map<unsigned char, long>       TrainTimerMap;
		TrainTimerMap							    m_timerIdentifierMap;
	};
};

#endif // !defined(UNSOLICITEDTRAINTIMER_H)

