/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/scada/Prototypes/ATSDemo/DummyControlStation.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"

#include "RunParams.h"
#include "DummyControlStation.h"

BOOL CALLBACK ETWProc( HWND hwnd, LPARAM lParam)
{
	if (hwnd==0)
	{
		return TRUE;
	}

	if (::GetParent(hwnd)==NULL)
	{
		HWND* pResult = reinterpret_cast<HWND*>(lParam);
		*pResult = hwnd;

		return FALSE;	
	}

	return TRUE;
}

HWND GetHandleFromThreadId(DWORD threadId)
{
	HWND finalResult=NULL;

	::EnumThreadWindows(threadId, &ETWProc, reinterpret_cast<LPARAM>(&finalResult));

	return finalResult;
}

void WaitForPeriod ( DWORD milliseconds )
{
	// local variable
	long cyclePeriod = milliseconds / 1000; // in seconds
    time_t endTime;

	// set up the DataViews polling idle time
	endTime = time ( NULL ) + cyclePeriod;

	// keep going forever
	while ( true)
	{
		// check if the current time is greater than the idle period
		if ( time ( NULL ) > endTime )
		{
			break;
		}
		else
		{
			::Sleep ( 100 );
		}
	}
}

DummyControlStation::DummyControlStation()
:
// default values
m_alarmEXE ( "C:\\ICONICS\\GENESIS-32\\Bin\\Gwx32.exe" ),
m_gwxEXE ( "C:\\ICONICS\\GENESIS-32\\Bin\\Gwx32.exe" ),
m_numScreens ( 1 ),
m_screenWidth ( 1280 ),
m_screenHeight ( 1024 ),
m_alarmNr ( "" ),
m_alarmBanner ( "" ),
m_leftSCADA ( "" ),
m_middleSCADA ( "" ),
m_rightSCADA ( "" ),
m_hSCADAProcess1 ( NULL ),
m_hSCADAProcess2 ( NULL ),
m_hSCADAProcess3 ( NULL ),
m_hAlarmProcess1 ( NULL ),
m_hAlarmProcess2 ( NULL ),
m_hAlarmProcess3 ( NULL )
{	
	// find number of physical screens from the command line
	if ( RunParams::GetInstance().IsSet ( "NumberScreens" ) )
	{
		m_numScreens = atoi ( RunParams::GetInstance().Get ( "NumberScreens" ).c_str() );
	}

	// find screen width from the command line
	if ( RunParams::GetInstance().IsSet ( "ScreenWidth" ) )
	{
		m_screenWidth = atoi ( RunParams::GetInstance().Get ( "ScreenWidth" ).c_str() );
	}

	// find screen height from the command line
	if ( RunParams::GetInstance().IsSet ( "ScreenHeight" ) )
	{
		m_screenHeight = atoi ( RunParams::GetInstance().Get ( "ScreenHeight" ).c_str() );
	}

	// find alarm exe height from the command line
	if ( RunParams::GetInstance().IsSet ( "AlarmExec" ) )
	{
		m_alarmEXE = RunParams::GetInstance().Get ( "AlarmExec" ).c_str();
	}

	// find gwx exe height from the command line
	if ( RunParams::GetInstance().IsSet ( "GwxExec" ) )
	{
		m_gwxEXE = RunParams::GetInstance().Get ( "GwxExec" ).c_str();
	}

	// find alarm banner height from the command line
	if ( RunParams::GetInstance().IsSet ( "AlarmHeight" ) )
	{
		m_alarmHeight = atoi ( RunParams::GetInstance().Get ( "AlarmHeight" ).c_str() );
	}

	// find ATS Alarm Nr
	if ( RunParams::GetInstance().IsSet ( "AlarmNr" ) )
	{
		m_alarmNr = RunParams::GetInstance().Get ( "AlarmNr" ).c_str();
	}

	// find ATS Alarm Page
	if ( RunParams::GetInstance().IsSet ( "AlarmBanner" ) )
	{
		m_alarmBanner = RunParams::GetInstance().Get ( "AlarmBanner" ).c_str();
	}

	// find ATS Alarm Page
	if ( RunParams::GetInstance().IsSet ( "LeftScada" ) )
	{
		m_leftSCADA = RunParams::GetInstance().Get ( "LeftScada" ).c_str();
	}

	// find ATS Alarm Page
	if ( RunParams::GetInstance().IsSet ( "MiddleScada" ) )
	{
		m_middleSCADA = RunParams::GetInstance().Get ( "MiddleScada" ).c_str();
	}

	// find ATS Alarm Page
	if ( RunParams::GetInstance().IsSet ( "RightScada" ) )
	{
		m_rightSCADA = RunParams::GetInstance().Get ( "RightScada" ).c_str();
	}

}

DummyControlStation::~DummyControlStation()
{
	if ( m_hSCADAProcess1 != NULL )
	{
		killWindow ( m_hSCADAProcess1 );
	}

	if ( m_hSCADAProcess2 != NULL )
	{
		killWindow ( m_hSCADAProcess2 );
	}

	if ( m_hSCADAProcess3 != NULL )
	{
		killWindow ( m_hSCADAProcess3 );
	}

	if ( m_hAlarmProcess1 != NULL )
	{
		SendMessage( m_hAlarmProcess1, WM_CLOSE, 0, 0 );
		m_hAlarmProcess1 = NULL;
	}

	if ( m_hAlarmProcess2 != NULL )
	{
		SendMessage( m_hAlarmProcess2, WM_CLOSE, 0, 0 );
		m_hAlarmProcess2 = NULL;
	}

	if ( m_hAlarmProcess3 != NULL )
	{
		SendMessage( m_hAlarmProcess3, WM_CLOSE, 0, 0 );
		m_hAlarmProcess3 = NULL;
	}
}

void DummyControlStation::launchApps()
{
	// show the application windows
	//showAlarm();
	//showSCADA();

	// show the application windows
	switch ( m_numScreens )
	{
	case 1:
		{
			showAppsOn1Screen();
			break;
		}

	case 3:
		{
			showAppsOnScreen1();
			WaitForPeriod ( 2000 );
			showAppsOnScreen2();
			WaitForPeriod ( 2000 );
			showAppsOnScreen3();
			break;
		}
	default:
		{
			break;
		}
	}

}

void DummyControlStation::showAppsOn1Screen()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	//*** Show the alarm banner first ***
	CString m_commandEditData;
	m_commandEditData.Format("%s %d %d %d %d %d %s %s",
							  m_alarmEXE, 
							  1, 
							  0,
							  m_screenWidth - 1,
							  m_alarmHeight,
							  0,
							  m_alarmNr,
							  m_alarmBanner);

	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		::WaitForInputIdle(m_processInfo.hProcess,10000);
		m_hAlarmProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	//*** Then show the SCADA screen ***
	m_commandEditData.ReleaseBuffer();
	WaitForPeriod ( 1000 );
	m_commandEditData.Format("%s %s %s",
							  m_gwxEXE, 
							  m_middleSCADA, 
							  "-runtime");
	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		//::WaitForInputIdle(m_processInfo.hProcess,10000);
		WaitForPeriod ( 5000 );
		m_hSCADAProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	m_commandEditData.ReleaseBuffer();
}

void DummyControlStation::showAppsOnScreen1()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	int smallScreenWidth = abs ( m_screenWidth / 3 );

	CString m_commandEditData;

	// *** show Alarm Banner ***
	m_commandEditData.Format("%s %d %d %d %d %d %s %s",
							  m_alarmEXE, 
							  1, 
							  0,
							  smallScreenWidth,
							  m_alarmHeight,
							  0,
							  m_alarmNr,
							  m_alarmBanner);

	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		::WaitForInputIdle(m_processInfo.hProcess,10000);
		m_hAlarmProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	// *** And then SCADA screen ***
	WaitForPeriod ( 1000 );
	m_commandEditData.ReleaseBuffer();
	m_commandEditData.Format("%s %s %s",
							  m_gwxEXE, 
							  m_leftSCADA, 
							  "-runtime");
	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		//::WaitForInputIdle(m_processInfo.hProcess,10000);
		WaitForPeriod ( 5000 );
		m_hSCADAProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	m_commandEditData.ReleaseBuffer();
}

void DummyControlStation::showAppsOnScreen2()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	int smallScreenWidth = abs ( m_screenWidth / 3 );

	CString m_commandEditData;

	// *** show Alarm Banner ***
	m_commandEditData.Format("%s %d %d %d %d %d %s %s",
							  m_alarmEXE, 
							  smallScreenWidth, 
							  0,
							  smallScreenWidth,
							  m_alarmHeight,
							  0,
							  m_alarmNr,
							  m_alarmBanner);

	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		::WaitForInputIdle(m_processInfo.hProcess,10000);
		m_hAlarmProcess2 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	// *** And then SCADA screen ***
	WaitForPeriod ( 1000 );
	m_commandEditData.ReleaseBuffer();
	m_commandEditData.Format("%s %s %s",
							  m_gwxEXE, 
							  m_middleSCADA, 
							  "-runtime");
	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		//::WaitForInputIdle(m_processInfo.hProcess,10000);
		WaitForPeriod ( 5000 );
		m_hSCADAProcess2 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	m_commandEditData.ReleaseBuffer();
}

void DummyControlStation::showAppsOnScreen3()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	int smallScreenWidth = abs ( m_screenWidth / 3 );

	CString m_commandEditData;

	// *** show Alarm Banner ***
	m_commandEditData.Format("%s %d %d %d %d %d %s %s",
							  m_alarmEXE, 
							  2*smallScreenWidth, 
							  0,
							  smallScreenWidth,
							  m_alarmHeight,
							  0,
							  m_alarmNr,
							  m_alarmBanner);

	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		::WaitForInputIdle(m_processInfo.hProcess,10000);
		m_hAlarmProcess3 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	// *** And then SCADA screen ***
	WaitForPeriod ( 1000 );
	m_commandEditData.ReleaseBuffer();
	m_commandEditData.Format("%s %s %s",
							  m_gwxEXE, 
							  m_rightSCADA, 
							  "-runtime");
	if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
	{
		//::WaitForInputIdle(m_processInfo.hProcess,10000);
		WaitForPeriod ( 5000 );
		m_hSCADAProcess3 = GetHandleFromThreadId(m_processInfo.dwThreadId);

		::CloseHandle(m_processInfo.hProcess);
		::CloseHandle(m_processInfo.hThread);
		m_processInfo.hProcess=NULL;
		m_processInfo.hThread=NULL;
	}
	else
	{
		// do nothing
	}

	m_commandEditData.ReleaseBuffer();
}

/*
void DummyControlStation::showSCADA()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	switch ( m_numScreens )
	{
	case 1:
		{
			CString m_commandEditData;
			m_commandEditData.Format("%s %s %s",
									  m_gwxEXE, 
									  m_middleSCADA, 
									  "-runtime");
			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				//::WaitForInputIdle(m_processInfo.hProcess,10000);
				WaitForPeriod ( 5000 );
				m_hSCADAProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
				m_commandEditData.ReleaseBuffer();
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			break;
		}
	case 3:
		{
			CString m_commandEditData;
			m_commandEditData.Format("%s %s %s",
									  m_gwxEXE, 
									  m_leftSCADA, 
									  "-runtime");
			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				//::WaitForInputIdle(m_processInfo.hProcess,10000);
				WaitForPeriod ( 5000 );
				m_hSCADAProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			m_commandEditData.Format("%s %s %s",
									  m_gwxEXE, 
									  m_middleSCADA, 
									  "-runtime");
			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				//::WaitForInputIdle(m_processInfo.hProcess,10000);
				WaitForPeriod ( 5000 );
				m_hSCADAProcess2 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			m_commandEditData.Format("%s %s %s",
									  m_gwxEXE, 
									  m_rightSCADA, 
									  "-runtime");
			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				//::WaitForInputIdle(m_processInfo.hProcess,10000);
				WaitForPeriod ( 5000 );
				m_hSCADAProcess3 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			break;
		}
	default:
		{
			break;
		}
	}
}

void DummyControlStation::showAlarm()
{
	STARTUPINFO m_startupInfo;
	memset(&m_startupInfo, 0, sizeof(m_startupInfo));
	m_startupInfo.cb = sizeof(m_startupInfo);
	m_startupInfo.dwFlags = STARTF_USEPOSITION;
	m_startupInfo.dwX = -1;
	m_startupInfo.dwY = 0;

	int smallScreenWidth = abs ( m_screenWidth / 3 );

	switch ( m_numScreens )
	{
	case 1:
		{
			CString m_commandEditData;
			m_commandEditData.Format("%s %d %d %d %d %d %s %s",
									  m_alarmEXE, 
									  1, 
									  0,
									  m_screenWidth - 1,
									  m_alarmHeight,
									  0,
									  m_alarmNr,
									  m_alarmBanner);

			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				::WaitForInputIdle(m_processInfo.hProcess,10000);
				m_hAlarmProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
				m_commandEditData.ReleaseBuffer();
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			break;
		}
	case 3:
		{
			CString m_commandEditData;
			m_commandEditData.Format("%s %d %d %d %d %d %s %s",
									  m_alarmEXE, 
									  1, 
									  0,
									  smallScreenWidth,
									  m_alarmHeight,
									  0,
									  m_alarmNr,
									  m_alarmBanner);

			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				::WaitForInputIdle(m_processInfo.hProcess,10000);
				m_hAlarmProcess1 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
				m_commandEditData.ReleaseBuffer();
			}
			else
			{
				// do nothing
			}

			WaitForPeriod ( 2000 );
			m_commandEditData.ReleaseBuffer();
			m_commandEditData.Format("%s %d %d %d %d %d %s %s",
									  m_alarmEXE, 
									  smallScreenWidth, 
									  0,
									  smallScreenWidth,
									  m_alarmHeight,
									  0,
									  m_alarmNr,
									  m_alarmBanner);

			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				::WaitForInputIdle(m_processInfo.hProcess,10000);
				m_hAlarmProcess2 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
				m_commandEditData.ReleaseBuffer();
			}
			else
			{
				// do nothing
			}

			WaitForPeriod ( 2000 );
			m_commandEditData.ReleaseBuffer();
			m_commandEditData.Format("%s %d %d %d %d %d %s %s",
									  m_alarmEXE, 
									  2*smallScreenWidth, 
									  0,
									  smallScreenWidth,
									  m_alarmHeight,
									  0,
									  m_alarmNr,
									  m_alarmBanner);

			if ( TRUE == ::CreateProcess(NULL, m_commandEditData.GetBuffer(0),NULL,NULL,FALSE, NULL, NULL, NULL,&m_startupInfo, &m_processInfo) )
			{
				::WaitForInputIdle(m_processInfo.hProcess,10000);
				m_hAlarmProcess3 = GetHandleFromThreadId(m_processInfo.dwThreadId);

				::CloseHandle(m_processInfo.hProcess);
				::CloseHandle(m_processInfo.hThread);
				m_processInfo.hProcess=NULL;
				m_processInfo.hThread=NULL;
			}
			else
			{
				// do nothing
			}

			m_commandEditData.ReleaseBuffer();
			break;
		}
	default:
		{
			break;
		}
	}
}
*/
void DummyControlStation::killWindow ( HWND & hWindow )
{

	DWORD processId = 0;
	DWORD threadId = 0;
	DWORD exitCode = 0;
	HANDLE hProcess = NULL;

	threadId = ::GetWindowThreadProcessId ( hWindow, &processId );

	if ( threadId != 0 )
	{
		hProcess = ::OpenProcess ( PROCESS_ALL_ACCESS, FALSE, processId );

		if ( hProcess != NULL )
		{
			if ( TRUE == ::GetExitCodeProcess ( hProcess, &exitCode ) )
			{
				if ( FALSE == ::TerminateProcess ( hProcess, exitCode ) )
				{
					// do nothing
				}
			}
			else
			{
				// do nothing
			}
		}
	}

	hWindow = NULL;
}