//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanNodeView.h $
// @author:  Bart Golab
// @version: $Revision: #9 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_)
#define AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanGeneralDetailPage.h"
#include "PlanStepDetailPage.h"
#include "PlanFlowchartPage.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"


namespace TA_Base_App
{
class CPlanBrowserDoc;
class PlanNode;


/////////////////////////////////////////////////////////////////////////////
// CPlanNodeView form view

class CPropertySheetWithDisable : public CPropertySheet
{
public:
    void EnablePage( int nPage, bool enable = true )
    {
        m_tab.EnableTabIndex(nPage, enable);
    }
protected:
	virtual BOOL OnInitDialog()
	{
		CPropertySheet::OnInitDialog();
        m_tab.SubclassDlgItem(GetTabControl()->GetDlgCtrlID(), this);
		return TRUE;
	}
private:
	TA_Base_Bus::CTabCtrlWithDisableEx m_tab;
};

class CPlanNodeView : public CScrollView
{
protected:
    CPlanNodeView();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE( CPlanNodeView )

    // Operations
public:

protected:
    CPlanBrowserDoc* GetDocument();
    PlanNode* GetPlanNode();


    // Attributes:
private:
    CPropertySheetWithDisable m_planPropertySheet;
    CPlanGeneralDetailPage m_generalDetailPage;
    CPlanStepDetailPage m_stepDetailPage;
    CPlanFlowchartPage m_flowchartPage;


    // Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanNodeView)
public:
    virtual void OnDraw( CDC* pDC );
    virtual void OnInitialUpdate();

protected:
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );
    // }}AFX_VIRTUAL

    // Implementation
protected:
    ~CPlanNodeView();
    // Generated message map functions
    // {{AFX_MSG(CPlanNodeView)
    afx_msg BOOL OnEraseBkgnd( CDC* pDC );
    afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg LRESULT OnSelectActivePlanInstance( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnResetTreeNodeView( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnRefreshPlanStepList( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnPlanNodeTypeUpgrade( WPARAM wParam, LPARAM lParam );
    afx_msg void OnDestroy();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_)
