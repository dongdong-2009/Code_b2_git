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
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanStepEditDPTimeCheckPage.h"
#include "DataPointTimeCheckStep.h"

static const char *const UNDEFINED_DATA_POINT_JUMP_STEP = "Undefined step";

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointTimeCheckPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDataPointTimeCheckPage, CPlanStepEditParameterPage)

CPlanStepEditDataPointTimeCheckPage::CPlanStepEditDataPointTimeCheckPage(DataPointTimeCheckStep* step) :
CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_DATA_POINT_TIME_CHECK_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointTimeCheckPage" );

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
    DDX_Control(pDX, IDC_LESS_THAN_TIME_SPAN_STEP, m_lessThanTimeSpanStepLabel);
    DDX_Control(pDX, IDC_MORE_THAN_TIME_SPAN_STEP, m_moreThanTimeSpanStepLabel);
    DDX_Control(pDX, IDC_DPCHTIME_DATA_POINT, m_dataPointLabel);
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

	CString prevDataPoint;
	m_dataPointLabel.GetWindowText(prevDataPoint);

    popupTree( ParamDataPoint, ParamDataPoint, m_dataPointLabel );

	CString currentDataPoint;
	m_dataPointLabel.GetWindowText(currentDataPoint);
	if (prevDataPoint != currentDataPoint)
	{
		DisplayParameters();
	}

	ResetStepDisplay();
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::OnPickMoreThanTimeSpanStepButton()
{
    FUNCTION_ENTRY( "OnPickMoreThanTimeSpanStepButton" );

    popupList( ParamSuccessStep, ParamSuccessStep, m_moreThanTimeSpanStepLabel );

    ResetStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::OnPickLessThanTimeSpanStepButton()
{
    FUNCTION_ENTRY( "OnPickLessThanTimeSpanStepButton" );

    popupList( ParamFailureStep, ParamFailureStep, m_lessThanTimeSpanStepLabel );

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

	m_timespanEdit.EnableWindow( enable );

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::string datapointName = "";
    std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamDataPoint ) == 0 )
		{
			datapointName = itLoop->Item;
			m_dataPointLabel.SetWindowText( datapointName.c_str() );
			m_dataPointType = DataPointConfigAccess::getDataPointType(datapointName);
		}
		else if ( itLoop->Name.compare( ParamSuccessStep ) == 0 )
		{
			m_moreThanTimeSpanStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamFailureStep ) == 0 )
		{
			m_lessThanTimeSpanStepLabel.SetWindowText( itLoop->Item.c_str() );
		}
	}

	m_timespanEdit.SetFormat("HH:mm:ss");
	fillTimePicker( ParamTimeSpan, m_timespanEdit );

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointTimeCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateTimeValue( ParamTimeSpan, m_timespanEdit );

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointTimeCheckPage::ResetStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled;

    if (m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED)
    {
        // Set the success and failure step display with step number and description
        m_moreThanTimeSpanStepLabel.SetWindowText( GetPlanStep()->getActivityParameter().displayValue(ParamSuccessStep).c_str() );
        m_lessThanTimeSpanStepLabel.SetWindowText( GetPlanStep()->getActivityParameter().displayValue(ParamFailureStep).c_str() );

        // Enable step picker button
        stepPickerEnabled = IsEditingEnabled();
    }
    else
    {
        if (IsEditingEnabled())
        {
            //Do nothing
        }

        // Set display to "undefined step"
        m_moreThanTimeSpanStepLabel.SetWindowText(UNDEFINED_DATA_POINT_JUMP_STEP);
        m_lessThanTimeSpanStepLabel.SetWindowText(UNDEFINED_DATA_POINT_JUMP_STEP);

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

}
