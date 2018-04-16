// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanCheckTimerPage.cpp : implementation file
//

#include "StdAfx.h"

#if defined (STEPTYPE_PLAN_CHECK_TIMER)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanSetTimerStep.h"
#include "PlanStepPlanSetTimerPage.h"
#include "Utilities.h"

// CPlanStepPlanSetTimerPage dialog

using namespace TA_Base_App;

IMPLEMENT_DYNAMIC(CPlanStepPlanSetTimerPage, CPlanStepEditParameterPage)

CPlanStepPlanSetTimerPage::CPlanStepPlanSetTimerPage(PlanSetTimerStep* step) :
	CPlanStepEditParameterPage(CPlanStepPlanSetTimerPage::IDD, step),
    m_timeSpanParameter(*step->getTimeSpanParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}

CPlanStepPlanSetTimerPage::~CPlanStepPlanSetTimerPage()
{
    FUNCTION_ENTRY( "~CPlanStepPlanSetTimerPage" );
    FUNCTION_EXIT;
}

void CPlanStepPlanSetTimerPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepPlanSetTimerPage)
    DDX_Control(pDX, IDC_TIMESPAN_PICKER, m_timeSpan);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlanStepPlanSetTimerPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepPlanSetTimerPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPlanStepPlanSetTimerPage message handlers

void TA_Base_App::CPlanStepPlanSetTimerPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    CWaitCursor waitCursor;
    UpdateData();

    m_timeSpan.SetFormat("HH:mm:ss");

    unsigned long timeSpanSecond = m_timeSpanParameter.getNumericValue();
    COleDateTimeSpan stepTimeSpan(0, 0, 0, (timeSpanSecond == 0)? 1 : timeSpanSecond);
    COleDateTime stepTimeSpanAsTime(COleDateTime::GetCurrentTime());
    stepTimeSpanAsTime.SetTime(stepTimeSpan.GetHours(), stepTimeSpan.GetMinutes(), stepTimeSpan.GetSeconds());
    m_timeSpan.SetTime(stepTimeSpanAsTime);

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanSetTimerPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    PlanSetTimerStep* step = dynamic_cast<PlanSetTimerStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the plan start time check step page");

    COleDateTime stepTimeSpanAsTime;
    m_timeSpan.GetTime(stepTimeSpanAsTime);
    COleDateTimeSpan stepTimeSpan(0, stepTimeSpanAsTime.GetHour(), stepTimeSpanAsTime.GetMinute(), stepTimeSpanAsTime.GetSecond());
    step->getTimeSpanParameter()->setValue(static_cast<TA_Base_Core::Seconds>(stepTimeSpan.GetTotalSeconds()));

    FUNCTION_EXIT;
}

#endif
