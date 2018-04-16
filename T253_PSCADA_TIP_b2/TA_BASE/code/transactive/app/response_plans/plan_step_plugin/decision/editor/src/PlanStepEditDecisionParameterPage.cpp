//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDecisionParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StepNumberStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/YesNoStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterStepNumberEditDlg.h"
#include "Resource.h"
#include "DecisionStep.h"
#include "PlanStepEditDecisionParameterPage.h"

namespace TA_Base_App
{

const unsigned int MIN_INTERACTIVE_STEP_TIMEOUT = 10;   // seconds
const unsigned int MAX_INTERACTIVE_STEP_TIMEOUT = 300;  // seconds
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDecisionParameterPage, CPlanStepEditParameterPage)

CPlanStepEditDecisionParameterPage::CPlanStepEditDecisionParameterPage(DecisionStep* step) :
CPlanStepEditParameterPage( IDD_PLAN_STEP_EDIT_DECISION_DLG, step )
{
    FUNCTION_ENTRY( "CPlanStepEditDecisionParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditDecisionParameterPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditDecisionParameterPage::~CPlanStepEditDecisionParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDecisionParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDecisionParameterPage)
    DDX_Control( pDX, IDC_DECISION_MESSAGE, m_decisionMessage );
    DDX_Control( pDX, IDC_DECISION_RESPONSE_TIMEOUT, m_responseTimeout );
    DDX_Control( pDX, IDC_NO_STEP, m_noStep );
    DDX_Control( pDX, IDC_YES_STEP, m_yesStep );
    DDX_Control( pDX, IDC_YES_BUTTON_LABEL, m_yesBtnLabel );
    DDX_Control( pDX, IDC_NO_BUTTON_LABEL, m_noBtnLabel );
    DDX_Control( pDX, IDC_PAUSE_BUTTON_LABEL, m_pauseBtnLabel );
    DDX_Control( pDX, IDC_NO_BUTTON_ENABLED, m_noBtnEnabled );
    DDX_Control( pDX, IDC_PAUSE_BUTTON_ENABLED, m_pauseBtnEnabled );
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDecisionParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDecisionParameterPage)
    ON_BN_CLICKED(IDC_NO_PICK_STEP_BUTTON, OnNoPickStepButton)
    ON_BN_CLICKED(IDC_YES_PICK_STEP_BUTTON, OnYesPickStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage message handlers

BOOL CPlanStepEditDecisionParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    m_responseTimeout.SetFormat( "mm:ss" );

    m_yesBtnLabel.SetLimitText( 10 );
    m_noBtnLabel.SetLimitText( 10 );
    m_pauseBtnLabel.SetLimitText( 10 );
    // hwo to m_responseTimeout.SetRange( MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT );

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditDecisionParameterPage::OnNoPickStepButton()
{
    FUNCTION_ENTRY( "OnNoPickStepButton" );

    popupList( ParamNoStep, ParamNoStep, m_noStep );

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::OnYesPickStepButton()
{
    FUNCTION_ENTRY( "OnYesPickStepButton" );

    popupList( ParamYesStep, ParamYesStep, m_yesStep );

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_decisionMessage.SetReadOnly( !enable );

    CWnd* yesStepButton = GetDlgItem(IDC_YES_PICK_STEP_BUTTON);
    if ( NULL != yesStepButton )
    {
        yesStepButton->EnableWindow(enable);
    }

    CWnd* noStepButton = GetDlgItem(IDC_NO_PICK_STEP_BUTTON);
    if ( NULL != noStepButton )
    {
        noStepButton->EnableWindow(enable);
    }

    m_responseTimeout.EnableWindow( enable );
    m_yesBtnLabel.SetReadOnly( !enable );
    m_noBtnLabel.SetReadOnly( !enable );
    m_pauseBtnLabel.SetReadOnly( !enable );
    m_noBtnEnabled.EnableWindow( enable );
    m_pauseBtnEnabled.EnableWindow( enable );

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    std::vector<ItemPair> vecItems;
    GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

    for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
    {
        if ( itLoop->Name.compare( ParamNoStep ) == 0 )
        {
            m_noStep.SetWindowText( itLoop->Item.c_str() );
        }
        else if ( itLoop->Name.compare( ParamYesStep ) == 0 )
        {
            m_yesStep.SetWindowText( itLoop->Item.c_str() );
        }
        else if ( itLoop->Name.compare( ParamDecisionMessage ) == 0 )
        {
            m_decisionMessage.SetWindowText( itLoop->Item.c_str() );
        }
        else if ( itLoop->Name.compare( ParamYesButtonLabel ) == 0 )
        {
            m_yesBtnLabel.SetWindowText( itLoop->Item.c_str() );
        }
        else if ( itLoop->Name.compare( ParamNoButtonLabel ) == 0 )
        {
            m_noBtnLabel.SetWindowText( itLoop->Item.c_str() );
        }
        else if ( itLoop->Name.compare( ParamPauseButtonLabel ) == 0 )
        {
            m_pauseBtnLabel.SetWindowText( itLoop->Item.c_str() );
        }
    }

    fillTimePicker( ParamResponseTimeout, m_responseTimeout );
    fillCheckbox( ParamNoButtonEnabled, m_noBtnEnabled );
    fillCheckbox( ParamPauseButtonEnabled, m_pauseBtnEnabled );

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    updateValue( ParamDecisionMessage, m_decisionMessage );
    updateValue( ParamYesButtonLabel, m_yesBtnLabel );
    updateValue( ParamNoButtonLabel, m_noBtnLabel );
    updateValue( ParamPauseButtonLabel, m_pauseBtnLabel );
    updateBooleanValue( ParamNoButtonLabel, m_noBtnEnabled );
    updateBooleanValue( ParamPauseButtonLabel, m_pauseBtnEnabled );
    updateTimeValue( ParamResponseTimeout, m_responseTimeout );
    
    FUNCTION_EXIT;
}

}