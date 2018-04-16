/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/PTTMonitorLibrary/src/RS232MonitorThread.cpp $
  * @author:  A. Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class extends the thread class to provide monitoring of the RS232 CTS
  * modem line.  It will initially be used by Radio and PA subsystem to monitor
  * the status of an attached PTT (Press To Talk) button.  However this class 
  * can easily be extended to monitor the other modem lines and data transmissions
  *
  * Note:  This class uses Windows specific functions and is not currently
  * portable to the Solaris/Linux platform.
  */

// RS232MonitorThread.cpp: implementation of the RS232MonitorThread class.
//
//////////////////////////////////////////////////////////////////////

#include "RS232MonitorThread.h"
#include "IRS232StatusChange.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#define STATUS_CHECK_TIMEOUT 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_Bus
{

	RS232MonitorThread::RS232MonitorThread(IRS232StatusChange&  callback,std::string port) 
		          : m_callback(callback), m_port(port), m_terminate(false)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	RS232MonitorThread::~RS232MonitorThread()
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		FUNCTION_EXIT;
	}


	void RS232MonitorThread::run()
	{
		FUNCTION_ENTRY("run");

		if(m_port.size() <= 0)
		{
			FUNCTION_EXIT;
			return;
		}

		HANDLE		hComm;
		OVERLAPPED	osStatus = {0};

		while (!m_terminate)
		{
			LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                 "Create file: %s",m_port.c_str() );
			// Connect to the comms port
			hComm = CreateFile( m_port.c_str(),  
								GENERIC_READ | GENERIC_WRITE, 
								0, 
								0, 
								OPEN_EXISTING,
								FILE_FLAG_OVERLAPPED,
								0);
			
			if (hComm == INVALID_HANDLE_VALUE)
			{
				unsigned long ret = GetLastError();
				LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					 "Create file %s failed. Error code: %d",m_port.c_str(),ret );

				if (ret==ERROR_ACCESS_DENIED)
				{
					// Port already in use
					m_callback.onPortBusy();
					sleep(1000);  // try again after 1 second
				}
				else
				{
					TA_ASSERT(false,"Error accessing comm port specified.");
				}
			}
			else
			{
                // TD #3308 - it doesn't look like this code does anything, but
                // the HP machines won't receive signals from the port unless we call
                // SetCommState with something (must reset the port in some way)
				DCB commState;
				if (GetCommState(hComm, &commState))
				{				
                	if (!SetCommState(hComm, &commState))
					{
						TA_ASSERT(FALSE,"Error resetting port state");
					}
				}

				// Create this OVERLAPPED structure hEvent.
				osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				TA_ASSERT(osStatus.hEvent != NULL,"Error creating status event.");

				if (!SetCommMask(hComm, EV_CTS))
				{
					// error setting communications mask
					TA_ASSERT(false,"Error setting communications event mask.");
				}
					
				m_callback.onPortReady();
				break;  // Go on to monitoring of the port
			}
		}


		// Request 
		DWORD dwCommEvent;
		bool  bWaitingOnStatus = false;

		while (!m_terminate)
		{
			if (!bWaitingOnStatus)
			{
			   if (!WaitCommEvent(hComm, &dwCommEvent, &osStatus))
			   {
				   if (GetLastError() == ERROR_IO_PENDING)
				   {
					  bWaitingOnStatus = true;
				   }
				   else
				   {
					   TA_ASSERT(false,"Error in WaitCommEvent");
				   }
				}
				else
				{
					// WaitCommEvent returned immediately.
					// Deal with status event as appropriate.
					onStatusChange(hComm); 
				}
			}

			// Check on overlapped operation.
			if (bWaitingOnStatus) 
			{
				// Wait a little while for an event to occur.
				DWORD dwRes = WaitForSingleObject(osStatus.hEvent, STATUS_CHECK_TIMEOUT);
				switch(dwRes)
				{
					// Event occurred.
					case WAIT_OBJECT_0: 
						DWORD dwOvRes;
						if (!GetOverlappedResult(hComm, &osStatus, &dwOvRes, FALSE))
						{
							// An error occurred in the overlapped operation;
							// call GetLastError to find out what it was
							// and abort if it is fatal.
							TA_ASSERT(false,"Error getting overlapped operation result.");
						}
						else
						{
							// Status event is stored in the event flag
							// specified in the original WaitCommEvent call.
							// Deal with the status event as appropriate.
							onStatusChange(hComm);
						}
						 // Set fWaitingOnStat flag to indicate that a new
						 // WaitCommEvent is to be issued.
						bWaitingOnStatus = false;
						break;

					case WAIT_TIMEOUT:
						// Operation isn't complete yet. fWaitingOnStatusHandle flag 
						// isn't changed since I'll loop back around and I don't want
						// to issue another WaitCommEvent until the first one finishes.
						break;                       

				 default:
					 // Error in the WaitForSingleObject; abort
					 // This indicates a problem with the OVERLAPPED structure's
					 // event handle.
					 TA_ASSERT(false,"Error when waiting for a single object");
					 m_terminate = true;
				}
 			}
		}

		// Close port
		if (osStatus.hEvent!=NULL)
		{
			CloseHandle(osStatus.hEvent);
		}

        if (hComm!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
		}
		m_terminate = false;

		FUNCTION_EXIT;
	}
			 
			 
			 
			 
	void RS232MonitorThread::onStatusChange(HANDLE hComm)
	{
		FUNCTION_ENTRY("onStatusChange");

		static bool firstCall = true;
		static unsigned long lastCTS   = false;

		DWORD dwModemStatus;
		BOOL  ok = GetCommModemStatus(hComm, &dwModemStatus);
		TA_ASSERT(ok,"Error getting modem status.");

		if (ok)
		{
			if ((firstCall)||(lastCTS != (MS_CTS_ON & dwModemStatus)))
			{
				firstCall = false;
				lastCTS = MS_CTS_ON & dwModemStatus;

				if (lastCTS!=0)
				{
					m_callback.onCTSEnable();
				}
				else
				{
					m_callback.onCTSDisable();
				}
			}
		}

		FUNCTION_EXIT;
	}
			 

	void RS232MonitorThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_terminate = true;
		FUNCTION_EXIT;
	}

}