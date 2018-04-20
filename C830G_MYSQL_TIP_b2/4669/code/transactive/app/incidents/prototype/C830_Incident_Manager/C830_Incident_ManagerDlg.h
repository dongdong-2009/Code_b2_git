/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/prototype/C830_Incident_Manager/C830_Incident_ManagerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// C830_Incident_ManagerDlg.h : header file
//

#if !defined(AFX_C830_INCIDENT_MANAGERDLG_H__E039CEE6_E57F_4BEB_9AEA_5DB73A8D8B77__INCLUDED_)
#define AFX_C830_INCIDENT_MANAGERDLG_H__E039CEE6_E57F_4BEB_9AEA_5DB73A8D8B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IncidentLogDialog.h"
#include "StationLogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CC830_Incident_ManagerDlg dialog

class CC830_Incident_ManagerDlg : public CDialog
{
// Construction
public:
	CC830_Incident_ManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CC830_Incident_ManagerDlg)
	enum { IDD = IDD_C830_INCIDENT_MANAGER_DIALOG };
	CWnd m_Pager;
	CListCtrl	m_list;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CC830_Incident_ManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

    CToolBarCtrl	  m_toolbar;
    CImageList        m_imageList;
    IncidentLogDialog   m_incidentSheet;
    StationLogSheet     m_stationLogSheet;

	// Generated message map functions
	//{{AFX_MSG(CC830_Incident_ManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNew();
    afx_msg void OnView();
    afx_msg void OnDelete();
    afx_msg void OnPrint();
    afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


   void CreateToolbar();

   void setupList();

   void putCrapInList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C830_INCIDENT_MANAGERDLG_H__E039CEE6_E57F_4BEB_9AEA_5DB73A8D8B77__INCLUDED_)
