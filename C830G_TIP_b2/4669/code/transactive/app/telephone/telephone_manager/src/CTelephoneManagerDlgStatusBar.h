/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneManagerDlgStatusBar.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Class defines the status bar that will be displayed in the CTelephoneManagerDlg
  */

#if !defined(AFX_CTELEPHONEMANAGERDLGSTATUSBAR_H__068BDB33_206C_459B_A41D_09727E0420C5__INCLUDED_)
#define AFX_CTELEPHONEMANAGERDLGSTATUSBAR_H__068BDB33_206C_459B_A41D_09727E0420C5__INCLUDED_

class CTelephoneManagerDlgStatusBar : public CStatusBar
{
// Construction
public:
	CTelephoneManagerDlgStatusBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelephoneManagerDlgStatusBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTelephoneManagerDlgStatusBar();

    // Override the parent's class to store text into m_lastMsgDisplayed
    BOOL SetPaneText( int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTelephoneManagerDlgStatusBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_lastMsgDisplayed;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTELEPHONEMANAGERDLGSTATUSBAR_H__068BDB33_206C_459B_A41D_09727E0420C5__INCLUDED_)
