//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditExpressionParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Evaluate Expression step.

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanStepEditExpressionPage.h"
#include "ExpressionStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditExpressionPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditExpressionPage, CPlanStepEditParameterPage)

CPlanStepEditExpressionPage::CPlanStepEditExpressionPage(ExpressionStep* step) :
   CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_EXPRESSION_DLG, step)
   {
    FUNCTION_ENTRY( "CPlanStepEditExpressionPage" );
    FUNCTION_EXIT;
}


CPlanStepEditExpressionPage::~CPlanStepEditExpressionPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditExpressionPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditExpressionPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditExpressionPage)
    DDX_Control(pDX, IDC_EXPRESSION_TEXT, m_expression);
    DDX_Control(pDX, IDC_FAIL_STEP, m_failStepLabel);
    DDX_Control(pDX, IDC_SUCCESS_STEP, m_successStepLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditExpressionPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditExpressionPage)
    ON_BN_CLICKED(IDC_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPlanStepEditExpressionPage::OnInitDialog ()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    CWnd* instMsgText = GetDlgItem (IDC_INSTRUCTIONS_TXT);
    if (instMsgText)
    {
        CString instructionMsg;
        instructionMsg.LoadString (IDS_EXPRESSION_STEP_PARAMETER_EVAL_INSTRUCTIONS);
        instMsgText->SetWindowText (instructionMsg);
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditExpressionPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

	popupList( ParamFailureStep, ParamFailureStep, m_failStepLabel );

    FUNCTION_EXIT;
}


void CPlanStepEditExpressionPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    popupList( ParamSuccessStep, ParamSuccessStep, m_successStepLabel );

    FUNCTION_EXIT;
}


void CPlanStepEditExpressionPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* expressionText =  GetDlgItem (IDC_EXPRESSION_TEXT);
    if (expressionText)
    {
        expressionText->EnableWindow(enable);
    }

    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(enable);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(enable);
    }

     CWnd* instMsgText = GetDlgItem (IDC_INSTRUCTIONS_TXT);
     if (instMsgText)
     {
         CString instructionMsg;
         instructionMsg.LoadString (IDS_EXPRESSION_STEP_PARAMETER_EVAL_INSTRUCTIONS);
         instMsgText->SetWindowText (instructionMsg);
     }

    FUNCTION_EXIT;
}


void CPlanStepEditExpressionPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamExpression ) == 0 )
		{
			m_expression.SetReadOnly (FALSE);
			m_expression.SetWindowText( itLoop->Item.c_str() );
			m_expression.ShowWindow (SW_SHOW);
		}
		else if ( itLoop->Name.compare( ParamSuccessStep ) == 0 )
		{
			m_successStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamFailureStep ) == 0 )
		{
			m_failStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
	}


    FUNCTION_EXIT;
}


void CPlanStepEditExpressionPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateValue( ParamExpression, m_expression );

    FUNCTION_EXIT;
}

}