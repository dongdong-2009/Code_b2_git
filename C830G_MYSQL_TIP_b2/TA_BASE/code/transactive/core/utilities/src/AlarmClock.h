/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/core/utilities/src/AlarmClock.h $
  * @author:   Robert Stagg
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class provides a mechanism to call a function after a set period of time asynchronously.
  */
#if !defined(ALARMCLOCK)
#define ALARMCLOCK

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Core
{
	class IAlarmClockHelper;
    class AlarmClock : Thread
    {
    public:
        
        /**
          * Constructor
          */
        AlarmClock(IAlarmClockHelper* alarmHelper);

        /**
          * Destructor
          */
        virtual ~AlarmClock();

        /**
          * start
          *
          * Sets and starts the timer. If called again it will cancel the current timer
		  */
        void start( const unsigned long milliseconds );

        /**
          * cancel
          *
          * Kills the alarm clock.
          */
        void cancel();   
        
		/**
		  * run
		  *
		  * starts the thread that times the clock
		  * This function should not be called directly.
		  */
		virtual void run();
		
		/**
		  * terminate
		  * 
		  * kills the clock before it goes off
		  * This function should not be called directly.
		  */
		virtual void terminate();    

    private:

        AlarmClock( const AlarmClock& theAlarmClock);
        AlarmClock& operator=(const AlarmClock&);
        
        unsigned long m_millisecondsToWait;
        IAlarmClockHelper* m_helper;
        bool m_cancelRequested;
        bool m_clockRunning;
        
		ReadWriteThreadLockable m_signalLock;
		Condition* m_clockCondition;
    };
}

#endif // !defined(TIMER)
