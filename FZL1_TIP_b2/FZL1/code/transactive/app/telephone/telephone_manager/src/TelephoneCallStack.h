/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneCallStack.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TelephoneCallStack.h: interface for the TelephoneCallStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELEPHONECALLSTACK_H__33CA3ED1_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_TELEPHONECALLSTACK_H__33CA3ED1_8829_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "CurrentCallStack.h"
#include "HistoryCallStack.h"
#include "OthersStackDlg.h"	// Added by ClassView

class TelephoneCallStack : public CTabDialogCtrl 
{
public:
	TelephoneCallStack();
	virtual ~TelephoneCallStack();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TelephoneCallStack)
	//}}AFX_VIRTUAL

private:
	OthersStackDlg		m_othersCallStackDlg;
	CurrentCallStack    m_currentCallStack;
	HistoryCallStack    m_historyCallStack;

	// Generated message map functions
protected:
	//{{AFX_MSG(TelephoneCallStack)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPHONECALLSTACK_H__33CA3ED1_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
