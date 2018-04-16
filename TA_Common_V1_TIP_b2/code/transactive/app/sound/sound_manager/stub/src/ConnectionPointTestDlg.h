/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/stub/src/ConnectionPointTestDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ConnectionPointTestDlg.h : header file
//

#if !defined(AFX_CONNECTIONPOINTTESTDLG_H__BA531140_7F84_47F5_8A49_1210C72CA74D__INCLUDED_)
#define AFX_CONNECTIONPOINTTESTDLG_H__BA531140_7F84_47F5_8A49_1210C72CA74D__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/sound/sound_client/src/ISoundObserver.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"


/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestDlg dialog

class ConnectionPointTestDlg : public CDialog,
							   public TA_Base_Bus::ISoundObserver
{
// Construction
public:
	ConnectionPointTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(ConnectionPointTestDlg)
	enum { IDD = IDD_CONNECTIONPOINTTEST_DIALOG };
	CListBox	m_messages;
	CSpinButtonCtrl	m_spinControl;
	CEdit	m_priority;
	CComboBox	m_category;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConnectionPointTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Event handlers;
	virtual void handleCategorySilenced(const std::string& category, bool isSilenced);
	virtual void handleAllSilenced(bool isSilenced);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(ConnectionPointTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void onSetSeverity();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSilence();
	afx_msg void OnUnsilence();
	afx_msg void OnUnsilenceAll();
	afx_msg void OnSilenceAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	TA_Base_Bus::SoundManagerProxy m_soundManager;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONPOINTTESTDLG_H__BA531140_7F84_47F5_8A49_1210C72CA74D__INCLUDED_)
