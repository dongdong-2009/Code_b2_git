/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/TreeStackPrototype/TreeListCtrlDemoDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TreeListCtrlDemoDlg.h : header file
//

#if !defined(AFX_TREELISTCTRLDEMODLG_H__DD5B463B_596F_11D3_9784_000000000000__INCLUDED_)
#define AFX_TREELISTCTRLDEMODLG_H__DD5B463B_596F_11D3_9784_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Don't forget to include TreeListCtrl.h !!!!!!!
#include "TreeListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoDlg dialog

class CTreeListCtrlDemoDlg : public CDialog
{
// Construction
public:
	CTreeListCtrlDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTreeListCtrlDemoDlg)
	enum { IDD = IDD_TREELISTCTRLDEMO_DIALOG1 };
	CListCtrl	m_HeldTree;
	CListCtrl	m_IncomingTree;
	CTreeListCtrl	m_CurrentTree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrlDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetupCurrentStack();
	void SetupHeldStack();
	void SetupIncomingStack();
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTreeListCtrlDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CImageList		m_il;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRLDEMODLG_H__DD5B463B_596F_11D3_9784_000000000000__INCLUDED_)
