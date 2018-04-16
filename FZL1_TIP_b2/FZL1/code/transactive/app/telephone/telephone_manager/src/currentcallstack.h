/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/currentcallstack.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_URRENTCALLSTACK_H__33CA3ED2_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_URRENTCALLSTACK_H__33CA3ED2_8829_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// urrentCallStack.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CurrentCallStack dialog

class CurrentCallStack : public CDialog
{
// Construction
public:
	CurrentCallStack(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CurrentCallStack)
	enum { IDD = IDD_TELEPHONESTACK };
	CListCtrl	m_holdCallStack;
	CListCtrl	m_incomingStack;
	CListCtrl	m_emergencyCallStack;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CurrentCallStack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CurrentCallStack)
	virtual BOOL OnInitDialog();
	afx_msg void OnAnswerIncom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URRENTCALLSTACK_H__33CA3ED2_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
