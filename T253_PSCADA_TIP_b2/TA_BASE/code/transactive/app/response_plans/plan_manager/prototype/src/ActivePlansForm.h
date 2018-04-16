/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ActivePlansForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_ACTIVEPLANSFORM_H__7656BF55_B383_4687_B6C6_DED7E50654CB__INCLUDED_)
#define AFX_ACTIVEPLANSFORM_H__7656BF55_B383_4687_B6C6_DED7E50654CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActivePlansForm.h : header file
//

#include "PvCommonDefs.h"
#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CActivePlansForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CActivePlansForm : public CFormView
{
protected:
    CActivePlansForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CActivePlansForm)

// Form Data
public:
    // {{AFX_DATA(CActivePlansForm)
    enum { IDD = IDD_ACTIVEPLANS_FORM };
    CNotes    m_notes;
    CListCtrl    m_allActivePlansListCtrl;
    // }}AFX_DATA

// Attributes
public:

protected:
    static bool PlanCompare(const PV::Plan *plan1, const PV::Plan *plan2);

// Operations
public:
    void PopulateData();

protected:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CActivePlansForm)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CActivePlansForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CActivePlansForm)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDblclkAllactiveplaninstList(NMHDR* pNMHDR, LRESULT* pResult);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVEPLANSFORM_H__7656BF55_B383_4687_B6C6_DED7E50654CB__INCLUDED_)
