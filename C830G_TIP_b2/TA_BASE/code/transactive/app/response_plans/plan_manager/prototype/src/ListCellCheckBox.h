/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ListCellCheckBox.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_LISTCELLCHECKBOX_H__FA4DC76B_75AE_4C59_81DA_46D14083640B__INCLUDED_)
#define AFX_LISTCELLCHECKBOX_H__FA4DC76B_75AE_4C59_81DA_46D14083640B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCellCheckBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCellCheckBox window

class CListCellCheckBox : public CButton
{
// Construction
public:
    CListCellCheckBox(CWnd *parent, CRect cellRect, int item, int subItem, CString text);

// Attributes
public:

protected:
    bool    m_esc;
    int     m_item;
    int     m_subItem;

// Operations
public:
    void Update();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CListCellCheckBox)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListCellCheckBox();

    // Generated message map functions
protected:
    // {{AFX_MSG(CListCellCheckBox)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    // }}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCELLCHECKBOX_H__FA4DC76B_75AE_4C59_81DA_46D14083640B__INCLUDED_)
