//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterPlanPathEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/plan_tree/src/PlanItem.h"
#include "PlanStepParameterPlanPathEditDlg.h"
#include "PlanPathStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"


namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterPlanPathEditDlg dialog

CPlanStepParameterPlanPathEditDlg::CPlanStepParameterPlanPathEditDlg(PlanPathStepParameter& parameter,bool canEdit)
    : CPlanStepParameterEditDlg(parameter,CPlanStepParameterPlanPathEditDlg::IDD, canEdit)
{
    FUNCTION_ENTRY( "CPlanStepParameterPlanPathEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterPlanPathEditDlg)
    m_planPath = _T("");
    // }}AFX_DATA_INIT

    // We want the plan selector tree to show only approved plans + the current plan (which should be unapproved).
    // We therefore specify the plan filter as PFT_APPROVED_PLANS but make the current plan unfilterable.
    // An example of where the user may want to select the current plan is in an Active Plan Check step that
    // they want to use to make sure that only one instance of the plan runs at a time.
    std::vector<TA_Base_Core::NodeId> nonFilterablePlans;
    nonFilterablePlans.push_back(parameter.getParentStep()->getParentPlan()->getPlanNodeId());

    m_planTree = new TA_Base_Bus::CPlanTreeCtrl(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS, nonFilterablePlans, false);

    FUNCTION_EXIT;
}


CPlanStepParameterPlanPathEditDlg::~CPlanStepParameterPlanPathEditDlg()
{
    FUNCTION_ENTRY( "~CPlanStepParameterPlanPathEditDlg" );

    delete m_planTree;

    FUNCTION_EXIT;
}


void CPlanStepParameterPlanPathEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterPlanPathEditDlg)
    DDX_Control(pDX, IDC_COMMON_PLAN_TREE, *m_planTree);
    DDX_Text(pDX, IDC_COMMON_PLAN_PATH, m_planPath);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterPlanPathEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterPlanPathEditDlg)
    ON_NOTIFY(TVN_SELCHANGED, IDC_COMMON_PLAN_TREE, OnPlanTreeItemSelected)
    ON_BN_CLICKED(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterPlanPathEditDlg message handlers

BOOL CPlanStepParameterPlanPathEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    // TODO: Add extra initialization here
    PlanPathStepParameter* parameter = dynamic_cast<PlanPathStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    // Select the appropriate plan if one is associated with the parameter.
    if (parameter->value() > 0)
    {
        m_planTree->setSelectedPlan(parameter->value());
    }

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterPlanPathEditDlg::OnPlanTreeItemSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnPlanTreeItemSelected" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    TA_Base_Bus::CPlanItem* planItem = m_planTree->getSelectedPlan();

    m_planPath = (planItem != NULL? _T(planItem->getPlanPath().c_str()) : _T(""));
    UpdateData(FALSE);

    CWnd* okButton = GetDlgItem(IDOK);
    if (okButton)
    {
        okButton->EnableWindow(planItem != NULL);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterPlanPathEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    PlanPathStepParameter* parameter = dynamic_cast<PlanPathStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    TA_Base_Bus::CPlanItem* planItem = m_planTree->getSelectedPlan();
    TA_ASSERT(planItem != NULL, "No plan selected in the tree");

    parameter->value(planItem->getPlanId());

    EndDialog(IDOK);

    FUNCTION_EXIT;
}

}