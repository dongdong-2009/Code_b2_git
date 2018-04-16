/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/TSAPINotificationThread.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_TSAPINOTIFICATIONTHREAD_H__C63D268E_B9F8_4215_B81A_3ED398665A2E__INCLUDED_)
#define AFX_TSAPINOTIFICATIONTHREAD_H__C63D268E_B9F8_4215_B81A_3ED398665A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TSAPINotificationThread.h : header file
//

#include "stdafx.h"


class CHiddenDialog;
namespace TA_App
{
	class TelephoneSwitch;
}
/////////////////////////////////////////////////////////////////////////////
// TSAPINotificationThread thread

class TSAPINotificationThread : public CWinThread
{
	DECLARE_DYNCREATE(TSAPINotificationThread)
protected:
	TSAPINotificationThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	HWND getWindowHandle();
	void setSwitch(TA_App::TelephoneSwitch* telephoneSwitch);
	void setHiddenDialogCreated();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TSAPINotificationThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TSAPINotificationThread();

	// Generated message map functions
	//{{AFX_MSG(TSAPINotificationThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CHiddenDialog* m_hiddenDialog;
	TA_App::TelephoneSwitch* m_switch;
	bool m_switchSet;
	bool m_hiddenDialogCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSAPINOTIFICATIONTHREAD_H__C63D268E_B9F8_4215_B81A_3ED398665A2E__INCLUDED_)
