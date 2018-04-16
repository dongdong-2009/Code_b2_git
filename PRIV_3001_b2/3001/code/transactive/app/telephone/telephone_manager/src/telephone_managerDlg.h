/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/telephone_managerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// telephone_managerDlg.h : header file
//

#if !defined(AFX_TELEPHONE_MANAGERDLG_H__57855048_8806_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_TELEPHONE_MANAGERDLG_H__57855048_8806_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\generic_gui\src\TransActiveDialog.h"
#include "TelephoneCallStack.h"
#include "TelephoneFacilitiesTab.h"
#include "dlgbars.h"

/////////////////////////////////////////////////////////////////////////////
// CTelephone_managerDlg dialog

class CTelephone_managerDlg : public TA_Bus::TransActiveDialog
{
// Construction
public:
	CTelephone_managerDlg(TA_Bus::IGUIAccess& controlClass);

	// Dialog Data
	//{{AFX_DATA(CTelephone_managerDlg)
	enum { IDD = IDD_TELEPHONE_MANAGER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelephone_managerDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

// Implementation
protected:
	HICON m_hIcon;

	void CreateToolbar();
	void EnableToolBar(bool bEnable);
	void EnableStatusBar(bool bEnable);
	void AddTextToButton(UINT buttonId, CString buttonLabel );


	// Generated message map functions
	//{{AFX_MSG(CTelephone_managerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void onViewToolbar();
	afx_msg void onViewStatusbar();
	afx_msg void nnHelpAboutTelephoneController();
	afx_msg void onLineForward();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
		
	TelephoneCallStack                   m_callstacks;
	TelephoneFacilitiesTab               m_facilitiestab;

	// Status bar
	CDlgStatusBar m_wndStatusBar;

    // Toolbar
	CDlgToolBar				m_toolBar;
	UINT					m_toolBarHeight;
	UINT                    m_statusBarHeight;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPHONE_MANAGERDLG_H__57855048_8806_11D7_B17D_0050BAB1D931__INCLUDED_)
