//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditJumpPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "stdafx.h"

#include "PlanStepEditJumpPage.h"
#include "JumpStep.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditJumpPage, CPlanStepEditParameterPage)

CPlanStepEditJumpPage::CPlanStepEditJumpPage(JumpStep* step)
  : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_JUMP_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditJumpPage" );
    FUNCTION_EXIT;
}


CPlanStepEditJumpPage::~CPlanStepEditJumpPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditJumpPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditJumpPage)
    DDX_Control(pDX, IDC_JUMP_STEP, m_jumpStep);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditJumpPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditJumpPage)
    ON_BN_CLICKED(IDC_PICK_STEP_BUTTON, OnPickStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage message handlers

void CPlanStepEditJumpPage::OnPickStepButton()
{
    FUNCTION_ENTRY( "OnPickStepButton" );

    // Select the required step
    popupList(ParamStepId, ParamStepId, m_jumpStep);

    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* targetStepButton = GetDlgItem(IDC_PICK_STEP_BUTTON);
    if (targetStepButton)
    {
        targetStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamStepId ) == 0 )
		{
			m_jumpStep.SetWindowText( itLoop->Item.c_str() );
			break;
		}
	}

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    FUNCTION_EXIT;
}

}
