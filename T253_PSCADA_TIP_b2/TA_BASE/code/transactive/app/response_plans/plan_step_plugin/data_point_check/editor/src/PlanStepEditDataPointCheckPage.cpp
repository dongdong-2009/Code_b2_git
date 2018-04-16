//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "PlanStepEditDataPointCheckPage.h"
#include "DataPointCheckStep.h"
#include "DataPointTestTypeStepParameter.h"

namespace TA_Base_App
{

static const EDataPointTest NUMERIC_DP_TESTS[] =
{
	EQUAL_TO_TEST,
    NOT_EQUAL_TO_TEST,
    LESS_THAN_TEST,
    GREATER_THAN_TEST,
    LESS_THAN_OR_EQUAL_TO_TEST,
    GREATER_THAN_OR_EQUAL_TO_TEST
};

static const EDataPointTest TEXT_DP_TESTS[] =
{
    EQUAL_TO_TEST,
    NOT_EQUAL_TO_TEST,
    LESS_THAN_TEST,
    GREATER_THAN_TEST,
    LESS_THAN_OR_EQUAL_TO_TEST,
    GREATER_THAN_OR_EQUAL_TO_TEST
};

static const EDataPointTest BOOLEAN_DP_TESTS[] =
{
    EQUAL_TO_TEST,
    NOT_EQUAL_TO_TEST
};

static const EDataPointTest UNDEFINED_DP_TESTS[] =
{
    UNDEFINED_TEST
};

static const char *const WRITEONLY_DATA_POINT = "Output data point - cannot check";
static const char *const UNDEFINED_DATA_POINT_VALUE = "Undefined data point value";
static const char *const UNDEFINED_DATA_POINT_JUMP_STEP = "Undefined step";



/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDataPointCheckPage, CPlanStepEditParameterPage)

CPlanStepEditDataPointCheckPage::CPlanStepEditDataPointCheckPage(DataPointCheckStep* step) :
   CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_DATA_POINT_CHECK_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}


CPlanStepEditDataPointCheckPage::~CPlanStepEditDataPointCheckPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDataPointCheckPage)
    DDX_Control(pDX, IDC_DPCH_DATA_POINT_VALUE_TEXT, m_dataPointValueTextEdit);
    DDX_Control(pDX, IDC_DPCH_DATA_POINT_VALUE_NUMBER, m_dataPointValueNumberEdit);
    DDX_Control(pDX, IDC_DPCH_DATA_POINT_VALUE_BOOL, m_dataPointValueSetCombo);
    DDX_Control(pDX, IDC_DPCH_TEST_TYPE, m_testTypeCombo);
    DDX_Control(pDX, IDC_DPCH_FAIL_STEP, m_failStepLabel);
    DDX_Control(pDX, IDC_DPCH_SUCCESS_STEP, m_successStepLabel);
    DDX_Control(pDX, IDC_DPCH_DATA_POINT, m_dataPointLabel);
    DDX_Control(pDX, IDC_DPCH_DATA_POINT_TYPE, m_dataPointTypeLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDataPointCheckPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDataPointCheckPage)
    ON_BN_CLICKED(IDC_DPCH_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    ON_BN_CLICKED(IDC_DPCH_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_DPCH_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage message handlers

void CPlanStepEditDataPointCheckPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

	CString prevDataPoint;
	m_dataPointLabel.GetWindowText(prevDataPoint);

	UpdateDataPointValueType();
	popupTree( ParamDataPoint, ParamDataPoint, m_dataPointLabel );

	CString currentDataPoint;
	m_dataPointLabel.GetWindowText(currentDataPoint);
	if (prevDataPoint != currentDataPoint)
	{
		DisplayParameters();
	}

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

    popupList( ParamFailureStep, ParamFailureStep, m_failStepLabel );

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    popupList( ParamSuccessStep, ParamSuccessStep, m_successStepLabel );

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* dataPointButton = GetDlgItem(IDC_DPCH_PICK_DATA_POINT_BUTTON);
    if (dataPointButton)
    {
        dataPointButton->EnableWindow(enable);
    }

    CWnd* successStepButton = GetDlgItem(IDC_DPCH_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(enable);
    }

    CWnd* failStepButton = GetDlgItem(IDC_DPCH_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(enable);
    }

    m_testTypeCombo.EnableWindow(enable);
    m_dataPointValueSetCombo.EnableWindow(enable);
    m_dataPointValueTextEdit.SetReadOnly(!enable);
    m_dataPointValueNumberEdit.SetReadOnly(!enable);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::string datapointName = "";
	std::string datapointValue = "";
	std::string datapointTestType = "";
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
		else if ( itLoop->Name.compare( ParamValueType ) == 0 )
		{
			m_dataPointTypeLabel.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamValue ) == 0 )
		{
			datapointValue = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamTestType ) == 0 )
		{
			datapointTestType = itLoop->Item;
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

	UpdateDataPointValueType();

    // Set the contents of the test type combo. Ensure that the currently selected type is applicable to
    // the data point.
    ResetDataPointTestTypeDisplay(datapointTestType);

    // Display the appropriate control for the type of data point. If the current value does not match the
    // type of data point, the content of the control is cleared.
	ResetDataPointValueDisplay(datapointName,datapointValue);

    ResetSuccessAndFailureStepDisplay();
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    UpdateDataPointValueType();
    UpdateDataPointValue();

	//Update test type
    int curSel = m_testTypeCombo.GetCurSel();
    unsigned int key = m_testTypeCombo.GetItemData(curSel);
    std::vector<unsigned int> vecSelectedKeys;
    vecSelectedKeys.push_back(key);
    std::string strDisplayValue = "";
    GetPlanStep()->getActivityParameter().getListPicker(ParamTestType)->pickFromList(ParamTestType, vecSelectedKeys, strDisplayValue);

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointCheckPage::UpdateDataPointValueType()
{
    FUNCTION_ENTRY( "UpdateDataPointValue" );

    CString dataPointValueStr;

    switch (m_dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
        case DataPointConfigAccess::DPT_SIGNED_INT16_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT16_SM:
        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
        case DataPointConfigAccess::DPT_SIGNED_INT32_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT32_SM:
        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
        {
			m_dataPointValueType = NUMERIC_DATA_POINT;
            break;
        }

        case DataPointConfigAccess::DPT_BOOLEAN:
        {
            m_dataPointValueType = BOOLEAN_DATA_POINT;
            break;
        }

        case DataPointConfigAccess::DPT_TEXT:
        {
			m_dataPointValueType = TEXT_DATA_POINT;
            break;
        }

        case DataPointConfigAccess::DPT_DERIVED_IN:
        {
			m_dataPointValueType = COMBINED_DATA_POINT;
            break;
        }

        default:
			m_dataPointValueType = UNDEFINED_DATA_POINT;
            break;
    }

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointCheckPage::ResetDataPointTestTypeDisplay(const std::string& datapointTestType)
{
    FUNCTION_ENTRY( "ResetDataPointTestTypeDisplay" );

	m_testTypeCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamTestType)->getInputListItems( ParamTestType, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_testTypeCombo.AddString( itLoop->Item.c_str() );
		m_testTypeCombo.SetItemData(nIdx, itLoop->ID );
	}

    // See if the new set of test types supports the currently configured test type. If it does then select it, otherwise
    // if current data point type is invalid then set the combo to "undefined test".
	int requiredTextTypeIdx = m_testTypeCombo.FindStringExact(-1, datapointTestType.c_str());
    if (requiredTextTypeIdx != CB_ERR)
    {
        m_testTypeCombo.SetCurSel(requiredTextTypeIdx);
    }
    else if (m_dataPointType == DataPointConfigAccess::DPT_UNDEFINED)
    {
        m_testTypeCombo.SetCurSel(0);
    }

    // Enable the combo box if editing is allowed and data point type is valid
    m_testTypeCombo.EnableWindow(IsEditingEnabled() && m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED);

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::ResetDataPointValueDisplay(const std::string& strDatapointName, const std::string& strDatapointValue)
{
    FUNCTION_ENTRY( "ResetDataPointValueDisplay" );

    m_dataPointValueSetCombo.ShowWindow(SW_HIDE);
    m_dataPointValueSetCombo.SetWindowText("");
    m_dataPointValueNumberEdit.ShowWindow(SW_HIDE);
    m_dataPointValueNumberEdit.SetWindowText("");
    m_dataPointValueTextEdit.ShowWindow(SW_HIDE);
    m_dataPointValueTextEdit.SetWindowText("");

    switch (m_dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
            m_dataPointValueNumberEdit.Reset(true, false);
            m_dataPointValueNumberEdit.SetRange(0.0, 65535.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 65535.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-32768.0, 32767.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32768.0 &&
                m_dataPointValue.numericValue().value <= 32767.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-32767.0, 32767.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT/* &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32767.0 &&
                m_dataPointValue.numericValue().value <= 32767.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
            m_dataPointValueNumberEdit.Reset(true, false);
            m_dataPointValueNumberEdit.SetRange(0.0, 4294967295.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 4294967295.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;


        case DataPointConfigAccess::DPT_SIGNED_INT32_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-2147483648.0, 2147483647.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483648.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT32_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-2147483647.0, 2147483647.0);
            if (m_dataPointValueType == NUMERIC_DATA_POINT/* &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483647.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
            m_dataPointValueNumberEdit.Reset(false, true);
            m_dataPointValueNumberEdit.SetRange(-3.4E+38, 3.4E+38);
            if (m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().value >= -3.4E+38 &&
                m_dataPointValue.numericValue().value <= 3.4E+38*/)
            {
                m_dataPointValueNumberEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_TEXT:
            m_dataPointValueTextEdit.LimitText(TA_Base_Bus::DPT_MAX_TEXT_LENGTH);
            m_dataPointValueTextEdit.SetReadOnly(FALSE);
            if (m_dataPointValueType == TEXT_DATA_POINT &&
                strcmp(strDatapointValue.c_str(), UNDEFINED_DATA_POINT_VALUE))
            {
                m_dataPointValueTextEdit.SetWindowText(strDatapointValue.c_str());
            }
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_BOOLEAN:
        case DataPointConfigAccess::DPT_DERIVED_IN:
        {
            std::string dataPointName = strDatapointName;
            DataPointConfigAccess::StateValueList derivedStates = DataPointConfigAccess::getDataPointStates(dataPointName);

            m_dataPointValueSetCombo.ResetContent();
            for (DataPointConfigAccess::StateValueList::iterator iter = derivedStates.begin(); iter != derivedStates.end(); iter++)
            {
                DataPointConfigAccess::StateValue derivedState(*iter);
                std::ostringstream stateLabel;
                stateLabel << derivedState.state << " : " << derivedState.description;

                int stateIdx = m_dataPointValueSetCombo.AddString(stateLabel.str().c_str());
                m_dataPointValueSetCombo.SetItemData(stateIdx, derivedState.state);

                int nNewValue = atoi(strDatapointValue.c_str());
                if (( m_dataPointValueType == COMBINED_DATA_POINT || m_dataPointValueType == BOOLEAN_DATA_POINT ) && ( nNewValue == derivedState.state ))
                {
                    m_dataPointValueSetCombo.SetCurSel(stateIdx);
                }
            }
            m_dataPointValueSetCombo.ShowWindow(SW_SHOW);
            break;
        }

        case DataPointConfigAccess::DPT_COMBINED_OUT:
            m_dataPointValueTextEdit.SetWindowText(WRITEONLY_DATA_POINT);
            m_dataPointValueTextEdit.SetReadOnly(TRUE);
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
            break;

        default:
            m_dataPointValueTextEdit.SetWindowText(UNDEFINED_DATA_POINT_VALUE);
            m_dataPointValueTextEdit.SetReadOnly(TRUE);
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointCheckPage::ResetSuccessAndFailureStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled;

    if (m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED)
    {
        // Set the success and failure step display with step number and description
        m_successStepLabel.SetWindowText(GetPlanStep()->getActivityParameter().displayValue(ParamSuccessStep).c_str());
		m_failStepLabel.SetWindowText(GetPlanStep()->getActivityParameter().displayValue(ParamFailureStep).c_str());

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
        m_successStepLabel.SetWindowText(UNDEFINED_DATA_POINT_JUMP_STEP);
        m_failStepLabel.SetWindowText(UNDEFINED_DATA_POINT_JUMP_STEP);

        // Disable step picker button
        stepPickerEnabled = false;
    }

    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_DPCH_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_DPCH_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointCheckPage::UpdateDataPointValue()
{
	FUNCTION_ENTRY( "UpdateDataPointValue" );

	switch( m_dataPointType )
	{
	case DataPointConfigAccess::DPT_UNSIGNED_INT16:
	case DataPointConfigAccess::DPT_SIGNED_INT16_2C:
	case DataPointConfigAccess::DPT_SIGNED_INT16_SM:
	case DataPointConfigAccess::DPT_UNSIGNED_INT32:
	case DataPointConfigAccess::DPT_SIGNED_INT32_2C:
	case DataPointConfigAccess::DPT_SIGNED_INT32_SM:
	case DataPointConfigAccess::DPT_IEEE_FLOAT32:
		{
			updateValue( ParamValue, m_dataPointValueNumberEdit );
			break;
		}

	case DataPointConfigAccess::DPT_BOOLEAN:
		{
			updateValue( ParamValue, m_dataPointValueSetCombo );
			break;
		}

	case DataPointConfigAccess::DPT_TEXT:
		{
			updateValue( ParamValue, m_dataPointValueTextEdit );;
			break;
		}

	case DataPointConfigAccess::DPT_COMBINED_OUT:
		{
			updateValue( ParamValue, m_dataPointValueSetCombo );
			break;
		}

	default:
		{
			updateValue( ParamValue, m_dataPointValueTextEdit );;
			break;
		}
	}

	FUNCTION_EXIT;
}

void CPlanStepEditDataPointCheckPage::updateValue( const std::string& strName, CWnd& refDispWnd )
{
	if( &refDispWnd == &m_dataPointValueSetCombo )
	{
		int curSel = m_dataPointValueSetCombo.GetCurSel();
		unsigned int key = m_dataPointValueSetCombo.GetItemData( curSel );
		std::vector<unsigned int> vecSelectedKeys;
		vecSelectedKeys.push_back( key );
		std::string strDisplayValue = "";
		GetPlanStep()->getActivityParameter().getListPicker( ParamValue )->pickFromList( ParamValue, vecSelectedKeys, strDisplayValue );
	}
	else
	{
		CPlanStepEditParameterPage::updateValue( strName, refDispWnd );
	}
}
}
