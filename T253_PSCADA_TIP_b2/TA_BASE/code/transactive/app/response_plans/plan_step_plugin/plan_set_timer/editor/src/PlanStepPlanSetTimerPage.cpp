// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanSetTimerPage.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanSetTimerStep.h"
#include "PlanStepPlanSetTimerPage.h"

// CPlanStepPlanSetTimerPage dialog

namespace TA_Base_App
{

IMPLEMENT_DYNAMIC(CPlanStepPlanSetTimerPage, CPlanStepEditParameterPage)

CPlanStepPlanSetTimerPage::CPlanStepPlanSetTimerPage(PlanSetTimerStep* step) :
	CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_PLAN_SET_TIMER_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointSetPage" );
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
    DDX_Control(pDX, IDC_PLANSETTI_TIMESPAN_PICKER, m_timeSpan);
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

    m_timeSpan.SetFormat("HH:mm:ss");
	fillTimePicker( ParamTime, m_timeSpan );

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepPlanSetTimerPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateTimeValue( ParamTime, m_timeSpan );

    FUNCTION_EXIT;
}

}
