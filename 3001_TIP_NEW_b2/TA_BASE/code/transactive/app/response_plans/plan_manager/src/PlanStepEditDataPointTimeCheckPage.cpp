//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanManager.h"
#include "PlanStepEditDataPointTimeCheckPage.h"
#include "DataPointTimeCheckStep.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *const UNDEFINED_DATA_POINT_JUMP_STEP = "Undefined step";


using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointTimeCheckPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDataPointTimeCheckPage, CPlanStepEditParameterPage)

CPlanStepEditDataPointTimeCheckPage::CPlanStepEditDataPointTimeCheckPage(DataPointTimeCheckStep* step) :
CPlanStepEditParameterPage(CPlanStepEditDataPointTimeCheckPage::IDD, step),
m_timespanParameter(*step->getTimeSpanParameter()),
m_dataPointParameter(*step->getDataPointParameter()),
m_moreThanTimeSpanParameter(*step->getSuccessParameter()),
m_lessThanTimeSpanParameter(*step->getFailParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointTimeCheckPage" );

    // {{AFX_DATA_INIT(CPlanStepEditDataPointTimeCheckPage)
    m_lessThanTimeSpanStepLabel = _T("");
    m_moreThanTimeSpanStepLabel = _T("");
    m_dataPointLabel = _T("");
    // }}AFX_DATA_INIT
    FUNCTION_EXIT;
}


CPlanStepEditDataPointTimeCheckPage::~CPlanStepEditDataPointTimeCheckPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDataPointTimeCheckPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDataPointTimeCheckPage)
    DDX_Text(pDX, IDC_LESS_THAN_TIME_SPAN_STEP, m_lessThanTimeSpanStepLabel);
    DDX_Text(pDX, IDC_MORE_THAN_TIME_SPAN_STEP, m_moreThanTimeSpanStepLabel);
    DDX_Text(pDX, IDC_DATA_POINT, m_dataPointLabel);
    DDX_Control(pDX, IDC_TIMESPAN_PICKER, m_timespanEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDataPointTimeCheckPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDataPointTimeCheckPage)
    ON_BN_CLICKED(IDC_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    ON_BN_CLICKED(IDC_PICK_LESS_THAN_TIME_SPAN_STEP_BUTTON, OnPickLessThanTimeSpanStepButton)
    ON_BN_CLICKED(IDC_PICK_MORE_THAN_TIME_SPAN_STEP_BUTTON, OnPickMoreThanTimeSpanStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointTimeCheckPage message handlers

void CPlanStepEditDataPointTimeCheckPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

    UpdateData(); // TD#1537

    std::string prevDataPoint = m_dataPointParameter.getDataPoint();

    m_dataPointParameter.editParameter();

    if (prevDataPoint != m_dataPointParameter.getDataPoint())
    {
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::OnPickMoreThanTimeSpanStepButton()
{
    FUNCTION_ENTRY( "OnPickMoreThanTimeSpanStepButton" );

    UpdateData(); // TD#1537

    m_moreThanTimeSpanParameter.editParameter();

    ResetStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::OnPickLessThanTimeSpanStepButton()
{
    FUNCTION_ENTRY( "OnPickLessThanTimeSpanStepButton" );

    UpdateData(); // TD#1537

    m_lessThanTimeSpanParameter.editParameter();

    ResetStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* dataPointButton = GetDlgItem(IDC_PICK_DATA_POINT_BUTTON);
    if (dataPointButton)
    {
        dataPointButton->EnableWindow(enable);
    }

    CWnd* successStepButton = GetDlgItem(IDC_PICK_LESS_THAN_TIME_SPAN_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(enable);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_MORE_THAN_TIME_SPAN_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    CWaitCursor waitCursor;
    UpdateData();

    m_timespanEdit.SetFormat("HH:mm:ss");

    unsigned long timeSpanSecond = m_timespanParameter.getNumericValue();
    COleDateTimeSpan stepTimeSpan(0, 0, 0, (timeSpanSecond == 0)? 1 : timeSpanSecond);
    COleDateTime stepTimeSpanAsTime(COleDateTime::GetCurrentTime());
    stepTimeSpanAsTime.SetTime(stepTimeSpan.GetHours(), stepTimeSpan.GetMinutes(), stepTimeSpan.GetSeconds());
    m_timespanEdit.SetTime(stepTimeSpanAsTime);

    // Set data point name display
    m_dataPointLabel= m_dataPointParameter.getDataPoint().c_str();

    // Set the success and failure step display to show the step number and description. If the data point
    // type is undefined disable the step picker buttons.
    ResetStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    DataPointTimeCheckStep* step = dynamic_cast<DataPointTimeCheckStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the data point time check step page");

    UpdateDataPointType();

    COleDateTime stepTimeSpanAsTime;
    m_timespanEdit.GetTime(stepTimeSpanAsTime);
    COleDateTimeSpan stepTimeSpan(0, stepTimeSpanAsTime.GetHour(), stepTimeSpanAsTime.GetMinute(), stepTimeSpanAsTime.GetSecond());
    step->getTimeSpanParameter()->setValue(static_cast<TA_Base_Core::Seconds>(stepTimeSpan.GetTotalSeconds()));

    step->getDataPointParameter()->setDataPoint(m_dataPointParameter.getDataPoint());
    step->getSuccessParameter()->setStepId(m_moreThanTimeSpanParameter.getStepId());
    step->getFailParameter()->setStepId(m_lessThanTimeSpanParameter.getStepId());

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointTimeCheckPage::UpdateDataPointType()
{
    FUNCTION_ENTRY( "UpdateDataPointType" );

    m_dataPointType = DataPointConfigAccess::DPT_UNDEFINED;
    std::string dataPointName = m_dataPointParameter.getDataPoint();

    if (!dataPointName.empty())
    {
        m_dataPointType = DataPointConfigAccess::getDataPointType(dataPointName);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::ResetStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled;

    if (m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED)
    {
        // Set the success and failure step display with step number and description
        m_moreThanTimeSpanStepLabel = m_moreThanTimeSpanParameter.getDestinationStepDescription().c_str();
        m_lessThanTimeSpanStepLabel = m_lessThanTimeSpanParameter.getDestinationStepDescription().c_str();

        // Enable step picker button
        stepPickerEnabled = IsEditingEnabled();
    }
    else
    {
        if (IsEditingEnabled())
        {
            // Reset success and failure step parameters.
            m_lessThanTimeSpanParameter.setStepId(0);
            m_moreThanTimeSpanParameter.setStepId(0);
        }

        // Set display to "undefined step"
        m_moreThanTimeSpanStepLabel = UNDEFINED_DATA_POINT_JUMP_STEP;
        m_lessThanTimeSpanStepLabel = UNDEFINED_DATA_POINT_JUMP_STEP;

        // Disable step picker button
        stepPickerEnabled = false;
    }

    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PICK_MORE_THAN_TIME_SPAN_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_LESS_THAN_TIME_SPAN_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_DATA_POINT_TIME_CHECK)
