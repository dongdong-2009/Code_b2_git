/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioInitMsgDlg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#if !defined(AFX_RADIOINITMSGDLG_H__C51B8FED_06DA_4799_926D_79FFB5B30717__INCLUDED_)
#define AFX_RADIOINITMSGDLG_H__C51B8FED_06DA_4799_926D_79FFB5B30717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioInitMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioInitMsgDlg dialog

class CRadioInitMsgDlg : public CDialog
{
// Construction
public:
	void setMsg(const CString &strMSg);
	CRadioInitMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRadioInitMsgDlg)
	enum { IDD = IDD_RADIO_INITIALISE };
	CStatic	m_msgStatic;
	//}}AFX_DATA


	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioInitMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRadioInitMsgDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOINITMSGDLG_H__C51B8FED_06DA_4799_926D_79FFB5B30717__INCLUDED_)
