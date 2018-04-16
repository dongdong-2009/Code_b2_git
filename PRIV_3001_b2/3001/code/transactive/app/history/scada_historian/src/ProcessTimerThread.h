#ifndef PROCESSTIMERTHREAD_H
#define PROCESSTIMERTHREAD_H

/**
  * ProcessTimerThread.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/ProcessTimerThread.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * <Insert description here>
  *
  */

#include <string>
#include "core/threads/src/Thread.h"
#include "app/history/scada_historian/src/HistoryConfig.h"


namespace TA_App
{
    //
    // Forward declaration
    //
    //class HistoryConfig;

	class ProcessTimerThread : public TA_Core::Thread
	{

	public:

		/**
		  * processTimerThread
		  * 
		  * Constructor.
		  *
		  */
		ProcessTimerThread() 
            : m_terminated (false), 
              m_controlMode(false), 
              m_threadStarted(false) {}
        

		/**
		  * ~processTimerThread
		  * 
		  * Destructor.
		  *
		  */
		virtual ~ProcessTimerThread() {}

		/**
		  * run
		  * 
		  * .
		  *
		  */
		virtual void run();

		/**
		  * terminate
		  * 
		  * .
		  *
		  */
		virtual void terminate();

		/**
		  * addHistoryConfig
		  * 
		  * 
		  *
		  * @return A pointer to the datapoint object
		  *
		  * @param context Context and name of the datapoint
		  *
		  * @exception TA_Core::TA_Exceptions::NameNotFoundException Thrown if
		  *			   there is a problem resolving the object
		  *
		  */
		void addHistoryConfig(HistoryConfig* historyCfg);

        /**
          * setToControl
          *
          * Sets the operation mode.
		  *
		  * @param isControl True: Control mode, False: Monitor mode
		  *
          */
        void setToControl(bool isControl)
        {
            m_controlMode = isControl;
        }

        /**
          * isThreadStarted
          *
          * Check to see if the thread has been started.
		  *
		  * @return True: Has started, False: Has not started
		  *
          */
        bool isThreadStarted()
        {
            return m_threadStarted;
        }

	private:	

		bool m_terminated;

        bool m_controlMode;

        bool m_threadStarted;

		std::vector<HistoryConfig*> m_dataCollectionCfg;

		void callProcedures(time_t currentTime);
	};

}

#endif // PROCESSTIMERTHREAD_H
