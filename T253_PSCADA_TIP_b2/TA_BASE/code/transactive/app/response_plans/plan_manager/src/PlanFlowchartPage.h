//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanFlowchartPage.h $
// @author:  Bart Golab
// @version: $Revision: #16 $
//
// Last modification: $DateTime: 2015/10/30 17:07:49 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#if !defined(AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_)
#define AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SinglePlanInstanceListCtrl.h"
#include "PlanInstanceComboBox.h"
#include "PlanDetailPage.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowEditor.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanFlowchartPage dialog

    class CPlanFlowchartPage : public CPlanDetailPage
    {
    // Construction
    public:
        CPlanFlowchartPage();   // standard constructor
        virtual ~CPlanFlowchartPage();

    // Operations
    public:
        int toActivityState( const int nStepStatus );
        void refreshInstanceStatus();
        void refreshWorkflowGraph();

    protected:
        virtual void SetPlanFilterButtonStates();
        virtual void ShowRelevantControls();
        virtual void RefreshControlContents();
        virtual void RefreshActiveInstances();
        virtual void PurgeActiveInstances();


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanFlowchartPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanFlowchartPage)
        afx_msg virtual BOOL OnInitDialog();
        afx_msg virtual BOOL OnSetActive();
        afx_msg virtual BOOL OnKillActive();
        afx_msg virtual void OnSize( UINT nType, int cx, int cy );
        afx_msg virtual void OnPlanInstanceComboItemSelected();
        afx_msg virtual void OnDestroy();
        afx_msg virtual LRESULT OnSelectActivePlanInstance( WPARAM wParam, LPARAM lParam );
        afx_msg virtual LRESULT OnActivePlanDetailUpdate( WPARAM wParam, LPARAM lParam );
        afx_msg virtual LRESULT OnActivePlanReset( WPARAM wParam, LPARAM lParam );
        afx_msg virtual LRESULT OnActiveStepDetailUpdate( WPARAM wParam, LPARAM lParam );
        afx_msg virtual LRESULT OnPlanSaving( WPARAM wParam, LPARAM lParam );
        afx_msg virtual LRESULT OnRefreshPlanSteps( WPARAM wParam, LPARAM lParam );
		afx_msg virtual LRESULT OnWorkFlowEditorCmd(WPARAM wParam, LPARAM lParam);
		afx_msg virtual LRESULT OnWorkFlowEditorCmdUIUpdate(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    protected:
        // {{AFX_DATA(CPlanFlowchartPage)
        enum { IDD = IDD_PLAN_FLOWCHART_DLG };
        CButton m_planStatusGroupBox;
        CStatic m_planInstanceComboLabel;
        CSinglePlanInstanceListCtrl m_planInstanceListCtrl;
        CPlanInstanceComboBox m_planInstanceCombo;
        // }}AFX_DATA

    protected:
        WorkflowEditor m_oWorkflowEditor;
        bool m_bViewMode;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_)
