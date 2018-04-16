/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/VariableSelector.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_VARIABLESELECTOR_H__02AC2CA0_4155_482A_8394_EFD06A9A9D1C__INCLUDED_)
#define AFX_VARIABLESELECTOR_H__02AC2CA0_4155_482A_8394_EFD06A9A9D1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/interlocking/InterlockingConfig/src/IAddVariable.h"
#include "Variable.h"
#include "resource.h"
// VariableSelector.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VariableSelector dialog

namespace TA_Base_Bus
{
    class VariableSelector : public CDialog
    {
    // Construction
    public:
	    VariableSelector(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
	    //{{AFX_DATA(VariableSelector)
	    enum { IDD = IDD_VariableSelector_DIALOG };
	    CListCtrl	m_selector;
	    //}}AFX_DATA

        IAddVariable* m_adder;
        Variable* m_variable;

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(VariableSelector)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(VariableSelector)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDblclkSelector(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCancel();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //namespace
#endif // !defined(AFX_VARIABLESELECTOR_H__02AC2CA0_4155_482A_8394_EFD06A9A9D1C__INCLUDED_)
