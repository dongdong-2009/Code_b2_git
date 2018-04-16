//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanTreeFrame.h $
// @author:  Bart Golab
// @version: $Revision: #6 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_)
#define AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanBrowserFrame.h"
#include "PlanTreeDoc.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class CPlanTreeView;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanTreeFrame frame

    class CPlanTreeFrame : public CPlanBrowserFrame
    {
        DECLARE_DYNCREATE(CPlanTreeFrame)
    public:
        CPlanTreeFrame();

    // Operations
    public:
        CPlanTreeDoc *GetDocument();

    protected:
        virtual CRuntimeClass *GetLeftPaneRuntimeClass();
        virtual CRuntimeClass *GetRightPaneRuntimeClass();
        virtual CPlanTreeView *GetLeftPane();
        virtual CView *GetRightPane();


    // Attributes
    protected:
        CSplitterWnd m_wndSplitter;


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanTreeFrame)
        public:
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanTreeFrame();

    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanTreeFrame)
        // }}AFX_MSG
        afx_msg LRESULT OnTreeNodeSelectionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_)
