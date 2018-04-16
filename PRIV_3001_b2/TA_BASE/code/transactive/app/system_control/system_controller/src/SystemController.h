/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_controller/src/SystemController.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is the starting point of the System Controller.
  * It starts the ProcessManager and SystemControllerMonitor
  * threads.
  *
  */

#ifndef SYSTEMCONTROLLER_H
#define SYSTEMCONTROLLER_H

#include "core/synchronisation/src/Semaphore.h"

namespace TA_Base_App
{
	class SignalEventHandler;		// Forward declaration
	class SystemControllerAdmin;
    class ShutdownExecutor;

    class SystemController
    {
    public:

		/**
		  * Constructor
		  */
		SystemController();

        /**
          * checkCommandLine
          *
          * Ensures that all command line arguments have been set. For
          * optional command line arguments, default RunParams are set if
          * they are missing from the command line.
          *
          * @return True if all necessary command line arguments have been
          *         set, and false otherwise.
          */
        bool checkCommandLine();
        
        /**
          * shutdown
          *
          * Shuts down the System Controller.
          */
        void shutdown();

        /** 
          * waitUntilShutdown
          *
          * Blocks on a semaphore until the System Controller is ready to 
          * be shutdown.
          */
		void waitUntilShutdown();

        /** 
          * shutDownRequested
          *
          * Informs this object that the System Controller is now meant to
          * shutdown.
          */
		void shutDownRequested();

        /**
          * getProcessId
          *
          * Returns the Process ID of the System Controller
          */
        unsigned long getProcessId();

        /**
          * Destructor
          */
        ~SystemController();

    private:

		SystemController( const SystemController& systemController);
	    SystemController& operator=(const SystemController&);

		SignalEventHandler* m_signalHandler;
        SystemControllerAdmin* m_systemControllerAdmin;
        ShutdownExecutor* m_shutdownExecutor;

        /**
          * The port to listen on.
          */
        int m_port;

        /**
          * Signalled when the System Controller should shutdown.
          */
        TA_Base_Core::Semaphore m_shutdownSemaphore;

    };
}

#endif
