//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanListView.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// Used for viewing all currently active plans.
//

#if !defined(AFX_ACTIVEPLANLISTVIEW_H__C8B5597F_6D37_4A26_96C4_26DBCF0DCFD3__INCLUDED_)
#define AFX_ACTIVEPLANLISTVIEW_H__C8B5597F_6D37_4A26_96C4_26DBCF0DCFD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanInstanceListCtrl.h"


namespace TA_Base_App
{
    class CActivePlanDoc;


    /////////////////////////////////////////////////////////////////////////////
    // CActivePlanListView view

    class CActivePlanListView : public CFormView
    {
    protected:
        CActivePlanListView();           // protected constructor used by dynamic creation
        ~CActivePlanListView();
        DECLARE_DYNCREATE(CActivePlanListView)


    // Operations
    public:
        CActivePlanDoc* GetDocument();


    // Attributes
    private:
        bool m_initialised;


    // Form Data
    private:
        // {{AFX_DATA(CActivePlanListView)
        enum { IDD = IDD_ACTIVE_PLAN_VIEW };
        CButton    m_allOwnersCheck;
        CButton    m_allLocationsCheck;
        CButton    m_activeInstancesGroupBox;
        CPlanInstanceListCtrl    m_planInstanceListCtrl;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CActivePlanListView)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
        // }}AFX_VIRTUAL

     // Generated message map functions
     protected:
        // {{AFX_MSG(CActivePlanListView)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnDblclkActiveInstanceList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnAllOwnersCheck();
        afx_msg void OnAllLocationsCheck();
    // }}AFX_MSG
        afx_msg LRESULT OnViewShown(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ACTIVEPLANLISTVIEW_H__C8B5597F_6D37_4A26_96C4_26DBCF0DCFD3__INCLUDED_)
