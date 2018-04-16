// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanCheckTimerPage.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanCheckTimerStep.h"
#include "PlanStepPlanCheckTimerPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

static const char *const UNDEFINED_NEXT_STEP = "Undefined step";
// CPlanStepPlanCheckTimerPage dialog

namespace TA_Base_App
{

IMPLEMENT_DYNAMIC(CPlanStepPlanCheckTimerPage, CPlanStepEditParameterPage)

CPlanStepPlanCheckTimerPage::CPlanStepPlanCheckTimerPage(PlanCheckTimerStep* step) :
	CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_PLAN_CHECK_TIMER_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}

CPlanStepPlanCheckTimerPage::~CPlanStepPlanCheckTimerPage()
{
    FUNCTION_ENTRY( "~CPlanStepPlanCheckTimerPage" );
    FUNCTION_EXIT;
}

void CPlanStepPlanCheckTimerPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepPlanCheckTimerPage)
    DDX_Control(pDX, IDC_PLANCHTI_TIMESPAN_PICKER, m_timeSpan);
    DDX_Control(pDX, IDC_PLANCHTI_FAIL_STEP, m_failStepLabel);
    DDX_Control(pDX, IDC_PLANCHTI_SUCCESS_STEP, m_successStepLabel);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlanStepPlanCheckTimerPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepPlanCheckTimerPage)
    ON_BN_CLICKED(IDC_PLANCHTI_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_PLANCHTI_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPlanStepPlanCheckTimerPage message handlers

void TA_Base_App::CPlanStepPlanCheckTimerPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

    popupList(ParamFailureStep, ParamFailureStep, m_failStepLabel);

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    popupList(ParamSuccessStep, ParamSuccessStep, m_successStepLabel);

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::ResetSuccessAndFailureStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled = IsEditingEnabled();


    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PLANCHTI_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PLANCHTI_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    m_timeSpan.SetFormat("HH:mm:ss");
    bool stepPickerEnabled;

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamSuccessStep ) == 0 )
		{
			m_successStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamFailureStep ) == 0 )
		{
			m_failStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
	}

	fillTimePicker( ParamTimeSpan, m_timeSpan );


    // Enable step picker button
    stepPickerEnabled = IsEditingEnabled();

    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PLANCHTI_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PLANCHTI_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateTimeValue( ParamTimeSpan, m_timeSpan );

    FUNCTION_EXIT;
}

}
