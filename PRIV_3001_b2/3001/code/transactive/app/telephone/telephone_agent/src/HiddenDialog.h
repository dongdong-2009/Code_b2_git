/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/HiddenDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_HIDDENDIALOG_H__6A9903C9_777C_4D10_9F29_E26C23DCDF6F__INCLUDED_)
#define AFX_HIDDENDIALOG_H__6A9903C9_777C_4D10_9F29_E26C23DCDF6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HiddenDialog.h : header file
//
//#include "TelephoneSwitch.h"

/////////////////////////////////////////////////////////////////////////////
// CHiddenDialog dialog
#define WM_ACSEVENT WM_USER + 500

namespace TA_App
{
	class TelephoneSwitch;
}
class TSAPINotificationThread;

//class TA_App::TelephoneSwitch;

class CHiddenDialog : public CDialog
{
// Construction
public:
	CHiddenDialog(TA_App::TelephoneSwitch* telephoneSwitch,CWnd* pParent = NULL);   // standard constructor

	void setThread(TSAPINotificationThread* thread){m_thread = thread;};
// Dialog Data
	//{{AFX_DATA(CHiddenDialog)
	enum { IDD = IDD_HIDDEN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHiddenDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHiddenDialog)
	afx_msg LRESULT OnACSEvent(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	TA_App::TelephoneSwitch* m_telephoneSwitch;
	TSAPINotificationThread* m_thread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIDDENDIALOG_H__6A9903C9_777C_4D10_9F29_E26C23DCDF6F__INCLUDED_)
