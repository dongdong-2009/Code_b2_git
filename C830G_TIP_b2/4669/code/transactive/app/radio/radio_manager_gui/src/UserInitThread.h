// UserInitThread.h: interface for the UserInitThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERINITTHREAD_H__0199B257_BCDC_4E6D_BEAE_14A9CC74F47E__INCLUDED_)
#define AFX_USERINITTHREAD_H__0199B257_BCDC_4E6D_BEAE_14A9CC74F47E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"

const unsigned long WM_KILLINITTHREAD_MESSAGE = WM_USER + 168;

class CRadioManagerDialog;



class UserInitThread : public TA_Base_Core::Thread  
{
public:

	enum  EThreadType
	{
		USERINIT = 0,
		CALLSTACKINIT,
		RADIOFACILITITIESINIT,
		SETRADIOSESSION
	};

	UserInitThread(CRadioManagerDialog& radioManagerDlg, EThreadType threadType);
	virtual ~UserInitThread();

	virtual void run();
	virtual void terminate();

private:

	CRadioManagerDialog&   m_radioManagerDlg;
	EThreadType  m_threadType;

};


#endif // !defined(AFX_USERINITTHREAD_H__0199B257_BCDC_4E6D_BEAE_14A9CC74F47E__INCLUDED_)
