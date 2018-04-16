/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/Prototype/src/RadioMonitorDlg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// RadioMonitorDlg.h : header file
//

#if !defined(AFX_RADIOMONITORDLG_H__2FA2A2C7_A4E1_412C_AD84_A1B882BD398F__INCLUDED_)
#define AFX_RADIOMONITORDLG_H__2FA2A2C7_A4E1_412C_AD84_A1B882BD398F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitorDlg dialog

class CRadioMonitorDlg : public CDialog
{
// Construction
public:
	CRadioMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRadioMonitorDlg)
	enum { IDD = IDD_RADIOMONITOR_DIALOG };
	CListCtrl	m_SearchList;
	CTreeCtrl	m_DirectoryTree;
	CListCtrl	m_callList;
	CListBox	m_monitorList;
	CString	m_searchEdit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioMonitorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRadioMonitorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOMONITORDLG_H__2FA2A2C7_A4E1_412C_AD84_A1B882BD398F__INCLUDED_)
