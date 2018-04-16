// UserInitThread.cpp: implementation of the UserInitThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UserInitThread.h"
#include "RadioManagerDlg.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


UserInitThread::UserInitThread(CRadioManagerDialog& radioManagerDlg, EThreadType threadType)
:m_radioManagerDlg(radioManagerDlg),
m_threadType(threadType)
{

}

UserInitThread::~UserInitThread()
{

}

void UserInitThread::run()
{
	switch(m_threadType)
	{
		
	case USERINIT:
		m_radioManagerDlg.UserInit();
		break;
		
	case CALLSTACKINIT:
		m_radioManagerDlg.m_CallStack.Init();	
		m_radioManagerDlg.m_CallStack.onUpdateRights();
		break;
		
	case RADIOFACILITITIESINIT:
		m_radioManagerDlg.m_RadioFacilities.Init();
		break;

// 	case SETRADIOSESSION:
// 		m_radioManagerDlg.setRadioSession();
// 		break;
		
	default:
		AfxMessageBox("Wrong thread type!");
		ASSERT(false);
	}

	PostMessage(m_radioManagerDlg.m_hWnd, WM_KILLINITTHREAD_MESSAGE, reinterpret_cast<WPARAM>(this), 0 );

}

void UserInitThread::terminate()
{

}
