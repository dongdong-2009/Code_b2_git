// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanCheckTimerPage.cpp : implementation file
//

#include "StdAfx.h"

#if defined (STEPTYPE_PLAN_CHECK_TIMER)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanCheckTimerStep.h"
#include "PlanStepPlanCheckTimerPage.h"
#include "PlanStepEditParameterPage.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"

static const char *const UNDEFINED_NEXT_STEP = "Undefined step";
// CPlanStepPlanCheckTimerPage dialog

using namespace TA_Base_App;

IMPLEMENT_DYNAMIC(CPlanStepPlanCheckTimerPage, CPlanStepEditParameterPage)

CPlanStepPlanCheckTimerPage::CPlanStepPlanCheckTimerPage(PlanCheckTimerStep* step) :
	CPlanStepEditParameterPage(CPlanStepPlanCheckTimerPage::IDD, step),
    m_timeSpanParameter(*step->getTimeSpanParameter()),
    m_successParameter(*step->getSuccessParameter()),
    m_failureParameter(*step->getFailParameter()),
    m_failStepLabel(_T("")),
    m_successStepLabel(_T(""))
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
    DDX_Control(pDX, IDC_TIMESPAN_PICKER, m_timeSpan);
    DDX_Text(pDX, IDC_FAIL_STEP, m_failStepLabel);
    DDX_Text(pDX, IDC_SUCCESS_STEP, m_successStepLabel);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlanStepPlanCheckTimerPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepPlanCheckTimerPage)
    ON_BN_CLICKED(IDC_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPlanStepPlanCheckTimerPage message handlers

void TA_Base_App::CPlanStepPlanCheckTimerPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

    UpdateData(); // TD#1537

    m_failureParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    UpdateData(); // TD#1537

    m_successParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::ResetSuccessAndFailureStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled;


    // Set the success and failure step display with step number and description
    m_successStepLabel = m_successParameter.getDestinationStepDescription().c_str();
    m_failStepLabel = m_failureParameter.getDestinationStepDescription().c_str();

    // Enable step picker button
    stepPickerEnabled = IsEditingEnabled();


    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    CWaitCursor waitCursor;
    UpdateData();

    m_timeSpan.SetFormat("HH:mm:ss");

    bool stepPickerEnabled;

    unsigned long timeSpanSecond = m_timeSpanParameter.getNumericValue();
    COleDateTimeSpan stepTimeSpan(0, 0, 0, (timeSpanSecond == 0)? 1 : timeSpanSecond);
    COleDateTime stepTimeSpanAsTime(COleDateTime::GetCurrentTime());
    stepTimeSpanAsTime.SetTime(stepTimeSpan.GetHours(), stepTimeSpan.GetMinutes(), stepTimeSpan.GetSeconds());
    m_timeSpan.SetTime(stepTimeSpanAsTime);

    // Set the success and failure step display with step number and description
    m_successStepLabel = ( m_successParameter.getStepId() == 0 )? UNDEFINED_NEXT_STEP : m_successParameter.getDestinationStepDescription().c_str();
    m_failStepLabel = ( m_failureParameter.getStepId() == 0 )? UNDEFINED_NEXT_STEP : m_failureParameter.getDestinationStepDescription().c_str();

    // Enable step picker button
    stepPickerEnabled = IsEditingEnabled();

    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanCheckTimerPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    PlanCheckTimerStep* step = dynamic_cast<PlanCheckTimerStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the plan start time check step page");

    COleDateTime stepTimeSpanAsTime;
    m_timeSpan.GetTime(stepTimeSpanAsTime);
    COleDateTimeSpan stepTimeSpan(0, stepTimeSpanAsTime.GetHour(), stepTimeSpanAsTime.GetMinute(), stepTimeSpanAsTime.GetSecond());
    step->getTimeSpanParameter()->setValue(static_cast<TA_Base_Core::Seconds>(stepTimeSpan.GetTotalSeconds()));

    step->getSuccessParameter()->setStepId(m_successParameter.getStepId());
    step->getFailParameter()->setStepId(m_failureParameter.getStepId());

    FUNCTION_EXIT;
}

#endif
