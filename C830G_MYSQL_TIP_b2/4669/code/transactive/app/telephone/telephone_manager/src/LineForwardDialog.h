/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/LineForwardDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_LINEFORWARDDIALOG_H__48356FD1_8B4C_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_LINEFORWARDDIALOG_H__48356FD1_8B4C_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineForwardDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LineForwardDialog dialog

class LineForwardDialog : public CDialog
{
// Construction
public:
	CString getStatusString();
	CString m_statusStr;
	LineForwardDialog(CWnd* pParent = NULL);   // standard constructor

	// Indicate if forwarding was set
	bool IsForward() { return m_forwardEnabled; }

// Dialog Data
	//{{AFX_DATA(LineForwardDialog)
	enum { IDD = IDD_CALL_FORWARD };
	CComboBox	m_numberCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LineForwardDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LineForwardDialog)
	virtual BOOL OnInitDialog();
	afx_msg void onOK();
	afx_msg void onForwardImmediate();
	afx_msg void onForwardDelayed();
	afx_msg void onForwardBusy();
	afx_msg void onForwardNever();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString buildStatusString();

	bool	m_forwardEnabled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEFORWARDDIALOG_H__48356FD1_8B4C_11D7_B17D_0050BAB1D931__INCLUDED_)
