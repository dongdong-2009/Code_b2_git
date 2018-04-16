//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTerminatePlanPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #7 $
//
// Last modification: $DateTime: 2015/01/22 11:33:34 $
// Last modified by:  $Author: huangjian $
//
// <description>


#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/Resource.h"
#include "app/response_plans/plan_manager/src/PlanStepEditTerminatePlanPage.h"
#include "app/response_plans/plan_manager/src/TerminatePlanStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTerminatePlanPage dialog

CPlanStepEditTerminatePlanPage::CPlanStepEditTerminatePlanPage(TerminatePlanStep* step)
    : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_TERMINATE_PLAN_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditTerminatePlanPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditTerminatePlanPage)
    DDX_Control(pDX, IDC_PLAN_PATH, m_planPathLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditTerminatePlanPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditTerminatePlanPage)
    ON_BN_CLICKED(IDC_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTerminatePlanPage message handlers

void CPlanStepEditTerminatePlanPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    popupTree(ParamPlan, ParamPlan, m_planPathLabel);

    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamPlan ) == 0 )
		{
			m_planPathLabel.SetWindowText( itLoop->Item.c_str() );
			break;
		}
	}

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    FUNCTION_EXIT;
}

}
