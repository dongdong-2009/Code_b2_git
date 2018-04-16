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

#include <algorithm>

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepEditParallelGatewayPage.h"
#include "ParallelGatewayStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditParallelGatewayPage, CPlanStepEditParameterPage)

CPlanStepEditParallelGatewayPage::CPlanStepEditParallelGatewayPage(ParallelGatewayStep* step)
  : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_PARALLEL_GATEWAY_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditParallelGatewayPage" );
    FUNCTION_EXIT;
}


CPlanStepEditParallelGatewayPage::~CPlanStepEditParallelGatewayPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditParallelGatewayPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditParallelGatewayPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditJumpPage)
    DDX_Control(pDX, IDC_PARALLEL_GATEWAY_INCOMING_STEP, m_incomingStepsLst);
	DDX_Control(pDX, IDC_PARALLEL_GATEWAY_OUTGOING_STEP, m_outgoingStepsLst);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditParallelGatewayPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditJumpPage)
    ON_BN_CLICKED(IDC_PICK_INCOMING_STEP_BUTTON, OnPickIncomingStepButton)
	ON_BN_CLICKED(IDC_PICK_OUTGOING_STEP_BUTTON, OnPickOutgoingStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage message handlers

void CPlanStepEditParallelGatewayPage::OnPickIncomingStepButton()
{
    FUNCTION_ENTRY( "OnPickIncomingStepButton" );

    // Select the required step
    popupList(ParamIncomingSteps, ParamIncomingSteps, m_incomingStepsLst);

    DisplayParameters();

    FUNCTION_EXIT;
}

void CPlanStepEditParallelGatewayPage::OnPickOutgoingStepButton()
{
	FUNCTION_ENTRY( "OnPickOutgoingStepButton" );

	// Select the required step
	popupList(ParamOutgoingSteps, ParamOutgoingSteps, m_outgoingStepsLst);

	DisplayParameters();

	FUNCTION_EXIT;
}

void CPlanStepEditParallelGatewayPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* incomingStepButton = GetDlgItem(IDC_PICK_INCOMING_STEP_BUTTON);
    if (incomingStepButton)
    {
        incomingStepButton->EnableWindow(enable);
    }

	CWnd* outgoingStepButton = GetDlgItem(IDC_PICK_OUTGOING_STEP_BUTTON);
	if (outgoingStepButton)
	{
		outgoingStepButton->EnableWindow(enable);
	}
    FUNCTION_EXIT;
}


void CPlanStepEditParallelGatewayPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    
    std::vector<ListItem> vecItems;
    std::vector<unsigned int> vecSelectKeys;
    unsigned int unMaxSelection = 0u;

    GetPlanStep()->getActivityParameter().getListPicker(ParamIncomingSteps)->getInputListItems(ParamIncomingSteps, vecItems, vecSelectKeys, unMaxSelection );

    m_incomingStepsLst.ResetContent();
	for ( std::vector<ListItem>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
        if ( vecSelectKeys.end() != std::find( vecSelectKeys.begin(), vecSelectKeys.end(), itLoop->ID ))
        {
            m_incomingStepsLst.AddString( itLoop->Item.c_str() );
        }
	}

    vecItems.clear();
    vecSelectKeys.clear();

    GetPlanStep()->getActivityParameter().getListPicker(ParamOutgoingSteps)->getInputListItems(ParamOutgoingSteps, vecItems, vecSelectKeys, unMaxSelection);

    m_outgoingStepsLst.ResetContent();
    for (std::vector<ListItem>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop)
    {
        if (vecSelectKeys.end() != std::find(vecSelectKeys.begin(), vecSelectKeys.end(), itLoop->ID))
        {
            m_outgoingStepsLst.AddString(itLoop->Item.c_str());
        }
    }

    FUNCTION_EXIT;
}

void CPlanStepEditParallelGatewayPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    FUNCTION_EXIT;
}

}
