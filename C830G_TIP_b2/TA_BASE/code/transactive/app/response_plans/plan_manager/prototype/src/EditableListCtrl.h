/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/EditableListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_)
#define AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditableListCtrl.h : header file
//

#include "ICellManager.h"


/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl window

class CEditableListCtrl : public CListCtrl
{
// Construction
public:
    CEditableListCtrl(ICellManager *cellManager);

// Attributes
protected:
   ICellManager *m_cellManager;
   CWnd *m_cellEditCtrl;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CEditableListCtrl)
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CEditableListCtrl();

    // Generated message map functions
protected:
    // {{AFX_MSG(CEditableListCtrl)
    afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    // }}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_)
