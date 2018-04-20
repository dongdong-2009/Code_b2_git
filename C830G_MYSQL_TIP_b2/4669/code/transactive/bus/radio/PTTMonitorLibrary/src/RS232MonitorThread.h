/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/PTTMonitorLibrary/src/RS232MonitorThread.h $
  * @author:  A. Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends the thread class to provide monitoring of the RS232 CTS
  * modem line.  It will initially be used by Radio and PA subsystem to monitor
  * the status of an attached PTT (Press To Talk) button.  However this class 
  * can easily be extended to monitor the other modem lines and data transmissions
  *
  * Note:  This class uses Windows specific functions and is not currently
  * portable to the Solaris/Linux platform.
  */

#if !defined(AFX_RS232MONITORTHREAD_H__43B5D500_0677_406B_821E_A27092628F5D__INCLUDED_)
#define AFX_RS232MONITORTHREAD_H__43B5D500_0677_406B_821E_A27092628F5D__INCLUDED_

// Only implemented for Windows platform
#ifndef WIN32
#error Only Windows platform supported for RS232MonitorThread.
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/thread.h"
#include <string>

namespace TA_IRS_Bus
{
	class IRS232StatusChange;

	class RS232MonitorThread : public TA_Base_Core::Thread  
	{
		public:

			/**
			  * RS232MonitorThread
			  * 
			  * Class constructor
			  *
			  * @param callback   Reference to interface to be called to pass back events.
			  * @param port       A string specifying the communications port (e.g. "COM1:")
			  */
			RS232MonitorThread(IRS232StatusChange& callback,std::string port);

			
			/**
			  * ~RS232MonitorThread
			  * 
			  * Class destructor
			  */
			virtual ~RS232MonitorThread();


			/**
			  * run
			  * 
			  * This class implements the threads main process which will be called by the framework 
			  * through the start method.  This method opens the communications port and starts the 
			  * monitoring of the CTS modem line
			  */
			void run();


			/**
			  * terminate
			  * 
			  * This method is called by the framework as part of the thread tremination process.
			  * It ends the communications monitoring process (run) and exits the thread.
			  */
			void terminate();

		private:

			/**
			  * onStatusChange
			  * 
			  * This method is called when a status change is detected on the modem CTS line.
			  * It checks for a change to state (to avoid bounce) and then calls the callback
			  * interface methods to advise the client 
			  */
			void onStatusChange(HANDLE hComm);

			
			// Class attributes
			IRS232StatusChange&   m_callback;
			std::string           m_port;
			bool				  m_terminate;

	};
}

#endif // !defined(AFX_RS232MONITORTHREAD_H__43B5D500_0677_406B_821E_A27092628F5D__INCLUDED_)
