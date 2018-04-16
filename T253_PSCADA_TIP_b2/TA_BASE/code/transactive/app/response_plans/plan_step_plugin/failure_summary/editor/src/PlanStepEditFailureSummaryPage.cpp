
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditFailureSummaryParameterPage.cpp $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

// PlanStepEditFailureSummaryParameterPage.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "PlanStepEditFailureSummaryPage.h"
#include "FailureSummaryStep.h"
#include "FailureCategoryStepParameter.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{

const std::string ALL_STEPS = "All Steps";
const std::string DATA_POINT_STEPS = "Datapoint Steps";
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditFailureSummaryParameterPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditFailureSummaryParameterPage, CPlanStepEditParameterPage)


CPlanStepEditFailureSummaryParameterPage::CPlanStepEditFailureSummaryParameterPage(FailureSummaryStep* step)
    : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_FAILURE_SUMMARY_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditFailureSummaryParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditFailureSummaryParameterPage)
    DDX_Control(pDX, IDC_FAILURE_SUMMARY_ALL_STEPS_RADIO, m_failureAllSteps);
	DDX_Control(pDX, IDC_FAILURE_SUMMARY_SET_DATA_POINT_STEPS_RADIO, m_failureSetDataPointSteps);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditFailureSummaryParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditFailureSummaryParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditFailureSummaryParameterPage message handlers

BOOL CPlanStepEditFailureSummaryParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    // TODO: Add extra initialization here
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditFailureSummaryParameterPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

	m_failureAllSteps.EnableWindow(enable);
	m_failureSetDataPointSteps.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamFailureCategory ) == 0 )
		{
			if (ALL_STEPS == itLoop->Item)
			{
				m_failureAllSteps.SetCheck(TRUE);
				m_failureSetDataPointSteps.SetCheck(FALSE);
			}
			else if (DATA_POINT_STEPS == itLoop->Item)
			{
				m_failureAllSteps.SetCheck(FALSE);
				m_failureSetDataPointSteps.SetCheck(TRUE);
			}
			else
			{
				m_failureAllSteps.SetCheck(FALSE);
				m_failureSetDataPointSteps.SetCheck(FALSE);
			}
			break;
		}
	}


    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    
	std::vector<unsigned int> vecSelectedKeys;
	if (m_failureAllSteps.GetCheck() == BST_CHECKED)
	{
		vecSelectedKeys.push_back(FailureSummaryStepParameter::SFC_ALL_STEPS);
	}
	else if (m_failureSetDataPointSteps.GetCheck() == BST_CHECKED)
	{
        vecSelectedKeys.push_back(FailureSummaryStepParameter::SFC_SET_DATA_POINT_STEPS);
	}
	else
	{
        vecSelectedKeys.push_back(FailureSummaryStepParameter::SFC_UNDEFINED);
	}

	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamFailureCategory)->pickFromList(ParamFailureCategory,vecSelectedKeys,strDisplayValue);

    FUNCTION_EXIT;
}

}