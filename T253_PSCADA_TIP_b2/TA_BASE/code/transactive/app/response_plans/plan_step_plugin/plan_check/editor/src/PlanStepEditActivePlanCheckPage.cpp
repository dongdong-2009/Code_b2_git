//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivePlanCheckPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "stdafx.h"
#include "resource.h"
#include "PlanStepEditActivePlanCheckPage.h"
#include "PlanCheckStep.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivePlanCheckPage dialog


CPlanStepEditActivePlanCheckPage::CPlanStepEditActivePlanCheckPage(PlanCheckStep* step)
    : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_ACTIVE_PLAN_CHECK_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditActivePlanCheckPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditActivePlanCheckPage)
    DDX_Control(pDX, IDC_PLAN_CHECK_NO_STEP, m_noStepLabel);
    DDX_Control(pDX, IDC_PLAN_CHECK_YES_STEP, m_yesStepLabel);
    DDX_Control(pDX, IDC_PLAN_CHECK_PLAN_PATH, m_planPathLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditActivePlanCheckPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditActivePlanCheckPage)
    ON_BN_CLICKED(IDC_PLAN_CHECK_PICK_NO_STEP_BUTTON, OnPickNoStepButton)
    ON_BN_CLICKED(IDC_PLAN_CHECK_PICK_YES_STEP_BUTTON, OnPickYesStepButton)
    ON_BN_CLICKED(IDC_PLAN_CHECK_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivePlanCheckPage message handlers

void CPlanStepEditActivePlanCheckPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PLAN_CHECK_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    CWnd* yesStepButton = GetDlgItem(IDC_PLAN_CHECK_PICK_YES_STEP_BUTTON);
    if (yesStepButton)
    {
        yesStepButton->EnableWindow(enable);
    }

    CWnd* noStepButton = GetDlgItem(IDC_PLAN_CHECK_PICK_NO_STEP_BUTTON);
    if (noStepButton)
    {
        noStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::DisplayParameters()
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
		else if ( itLoop->Name.compare( ParamYesStep ) == 0 )
		{
			m_yesStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamNoStep ) == 0 )
		{
			m_noStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
	}

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickNoStepButton()
{
    FUNCTION_ENTRY( "OnPickNoStepButton" );

    popupList(ParamNoStep, ParamNoStep, m_noStepLabel);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickYesStepButton()
{
    FUNCTION_ENTRY( "OnPickYesStepButton" );

    popupList(ParamYesStep, ParamYesStep, m_yesStepLabel);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    popupTree(ParamPlan, ParamPlan, m_planPathLabel);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    FUNCTION_EXIT;
}

}
