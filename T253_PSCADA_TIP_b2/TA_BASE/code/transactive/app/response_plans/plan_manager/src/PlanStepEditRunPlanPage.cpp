//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditRunPlanPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #8 $
//
// Last modification: $DateTime: 2015/01/23 10:43:26 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/PlanStepEditRunPlanPage.h"
#include "app/response_plans/plan_manager/src/RunPlanStep.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditRunPlanPage dialog

CPlanStepEditRunPlanPage::CPlanStepEditRunPlanPage(RunPlanStep* step)
: CPlanStepEditParameterPage( IDD_PLAN_STEP_EDIT_RUN_PLAN_DLG, step )
{
    FUNCTION_ENTRY( "CPlanStepEditRunPlanPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditRunPlanPage)
    DDX_Control(pDX, IDC_PLAN_PATH, m_planPathLabel);
    DDX_Control(pDX, IDC_MANUAL_LAUNCH, m_manualLaunch);
    DDX_Control(pDX, IDC_WAIT_FOR_COMPLETION, m_waitComplete);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditRunPlanPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditRunPlanPage)
    ON_BN_CLICKED(IDC_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditRunPlanPage message handlers

BOOL CPlanStepEditRunPlanPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }
    
	DisplayParameters();

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditRunPlanPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    popupTree(ParamPlan, ParamPlan, m_planPathLabel);

    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    m_manualLaunch.EnableWindow(enable);
    m_waitComplete.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamPlan ) == 0 )
		{
			m_planPathLabel.SetWindowText( itLoop->Item.c_str() );
		}
	}

	fillCheckbox( ParamManualLaunch, m_manualLaunch );
	fillCheckbox( ParamWaitForCompletion, m_waitComplete );

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateBooleanValue( ParamManualLaunch, m_manualLaunch );
	updateBooleanValue( ParamWaitForCompletion, m_waitComplete );

    FUNCTION_EXIT;
}

}