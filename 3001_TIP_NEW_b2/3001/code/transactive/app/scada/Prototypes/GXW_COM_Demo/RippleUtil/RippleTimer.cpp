/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/Prototypes/GXW_COM_Demo/RippleUtil/RippleTimer.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// RippleTimer.cpp : Implementation of CRippleTimer
#include "stdafx.h"
#include "RippleUtil.h"
#include "RippleTimer.h"

/////////////////////////////////////////////////////////////////////////////
// CRippleTimer

CRippleTimer *theObj = NULL;

STDMETHODIMP CRippleTimer::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IRippleTimer
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CRippleTimer::SetPeriod(long period)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_period = period;

	return S_OK;
}

HRESULT CRippleTimer::FinalConstruct( )
{
	::OutputDebugString("CRippleTimer::FinalConstruct()\n");
	m_hThread = 0;
	m_period = 1000; // 1 second default period.
	return S_OK;
}

void CRippleTimer::FinalRelease()
{
	::OutputDebugString("CRippleTimer::FinalRelease()\n");
	StopTimerThread();
}


STDMETHODIMP CRippleTimer::Start()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	::OutputDebugString("CRippleTimer::Start()\n");
	StartTimerThread();

	return S_OK;
}

STDMETHODIMP CRippleTimer::Stop()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	::OutputDebugString("CRippleTimer::Stop()\n");
	StopTimerThread();

	return S_OK;
}

void CRippleTimer::StopTimerThread()
{
	// if we have a timer thread running
	if (0 != m_hThread)
	{
		// signal the thread to stop
		m_bStop = true;
		// wait for it to stop
		if (::WaitForSingleObject(m_hThread,m_period) != WAIT_OBJECT_0)
		{
			// if we timed out, forcefully terminate it
			::TerminateThread(m_hThread,0);
		}
		::CloseHandle(m_hThread);
		m_hThread = 0;
	}
}

DWORD WINAPI TimerThreadProc(LPVOID lpParameter)
{
	::OutputDebugString("TimerThreadProc() Started\n");
	// get pointer to the obj that created us
	CRippleTimer *pObj = (CRippleTimer*)lpParameter;

	// while not signalled to stop
	while (! pObj->m_bStop)
	{
		Sleep(pObj->m_period); // sleep the required amount of time
		::OutputDebugString("TimerThreadProc() Firing Tick Event\n");
		pObj->Fire_Tick(); // fire a tick event to the client
	}

	::OutputDebugString("TimerThreadProc() Exiting\n");
	return 0;
}

bool CRippleTimer::StartTimerThread()
{
	m_hThread = ::CreateThread(NULL,0,TimerThreadProc,this,0,0);
	return ((m_hThread != 0) ? true : false);
}
