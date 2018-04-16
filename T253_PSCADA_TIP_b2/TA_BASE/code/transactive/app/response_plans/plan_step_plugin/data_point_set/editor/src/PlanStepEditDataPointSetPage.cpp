//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointSetPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Data Point Set step.

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanStepEditDataPointSetPage.h"
#include "DataPointSetStep.h"

static const char* const READONLY_DATA_POINT = "Input data point - cannot set";
static const char* const UNDEFINED_DATA_POINT_VALUE = "Undefined data point value";


namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointSetPage property page

IMPLEMENT_DYNAMIC( CPlanStepEditDataPointSetPage, CPlanStepEditParameterPage )

CPlanStepEditDataPointSetPage::CPlanStepEditDataPointSetPage( DataPointSetStep* step ) :
    CPlanStepEditParameterPage( IDD_PLAN_STEP_EDIT_DATA_POINT_SET_DLG, step )
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointSetPage" );
    FUNCTION_EXIT;
}


CPlanStepEditDataPointSetPage::~CPlanStepEditDataPointSetPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDataPointSetPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::DoDataExchange( CDataExchange* pDX )
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange( pDX );
    // {{AFX_DATA_MAP(CPlanStepEditDataPointSetPage)
    DDX_Control( pDX, IDC_DPSET_DATA_POINT_VALUE_TEXT, m_dataPointValueTextEdit );
    DDX_Control( pDX, IDC_DPSET_DATA_POINT_VALUE_NUMBER, m_dataPointValueNumberEdit );
    DDX_Control( pDX, IDC_DPSET_DATA_POINT_VALUE_BOOL, m_dataPointValueSetCombo );
    DDX_Control( pDX, IDC_DPSET_DATA_POINT, m_dataPointLabel );
    DDX_Control( pDX, IDC_DPSET_DATA_POINT_TYPE, m_dataPointTypeLabel );
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP( CPlanStepEditDataPointSetPage, CPlanStepEditParameterPage )
    // {{AFX_MSG_MAP(CPlanStepEditDataPointSetPage)
    ON_BN_CLICKED( IDC_DPSET_PICK_DATA_POINT_BUTTON, OnPickDataPointButton )
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage message handlers

void CPlanStepEditDataPointSetPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

    CString prevDataPoint;
    m_dataPointLabel.GetWindowText( prevDataPoint );

    UpdateDataPointValueType();

    popupTree( ParamDataPoint, ParamDataPoint, m_dataPointLabel );

    CString currentDataPoint;
    m_dataPointLabel.GetWindowText( currentDataPoint );
    if( prevDataPoint != currentDataPoint )
    {
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::EnableParameterControls( bool enable ) // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* dataPointButton = GetDlgItem( IDC_DPSET_PICK_DATA_POINT_BUTTON );
    if( dataPointButton )
    {
        dataPointButton->EnableWindow( enable );
    }

    m_dataPointValueSetCombo.EnableWindow( enable );
    m_dataPointValueTextEdit.SetReadOnly( !enable );
    m_dataPointValueNumberEdit.SetReadOnly( !enable );

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    std::string datapointName = "";
    std::string datapointNewValue = "";
    std::vector<ItemPair> vecItems;
    GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

    for( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
    {
        if( itLoop->Name.compare( ParamDataPoint ) == 0 )
        {
            datapointName = itLoop->Item;
            m_dataPointLabel.SetWindowText( datapointName.c_str() );
            m_dataPointType = DataPointConfigAccess::getDataPointType( datapointName );
        }
        else if( itLoop->Name.compare( ParamNewValueType ) == 0 )
        {
            m_dataPointTypeLabel.SetWindowText( itLoop->Item.c_str() );
        }
        else if( itLoop->Name.compare( ParamNewValue ) == 0 )
        {
            datapointNewValue = itLoop->Item;
        }
    }

    UpdateDataPointValueType();
    ResetDataPointValueDisplay( datapointName, datapointNewValue );

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    UpdateDataPointValueType();
    UpdateDataPointValue();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::UpdateDataPointValueType()
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

        case DataPointConfigAccess::DPT_COMBINED_OUT:
        {
            m_dataPointValueType = COMBINED_DATA_POINT;
            break;
        }

        default:
            m_dataPointValueType = UNDEFINED_DATA_POINT;
    }

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointSetPage::ResetDataPointValueDisplay( const std::string& strDatapointName, const std::string& strDatapointNewValue )
{
    FUNCTION_ENTRY( "ResetDataPointValueDisplay" );

    m_dataPointValueSetCombo.ShowWindow( SW_HIDE );
    m_dataPointValueSetCombo.SetWindowText( "" );
    m_dataPointValueNumberEdit.ShowWindow( SW_HIDE );
    m_dataPointValueNumberEdit.SetWindowText( "" );
    m_dataPointValueTextEdit.ShowWindow( SW_HIDE );
    m_dataPointValueTextEdit.SetWindowText( "" );

    switch( m_dataPointType )
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
            m_dataPointValueNumberEdit.Reset( true, false );
            m_dataPointValueNumberEdit.SetRange( 0.0, 65535.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 65535.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset( true, true );
            m_dataPointValueNumberEdit.SetRange( -32768.0, 32767.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32768.0 &&
                m_dataPointValue.numericValue().value <= 32767.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset( true, true );
            m_dataPointValueNumberEdit.SetRange( -32767.0, 32767.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT/* &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32767.0 &&
                m_dataPointValue.numericValue().value <= 32767.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
            m_dataPointValueNumberEdit.Reset( true, false );
            m_dataPointValueNumberEdit.SetRange( 0.0, 4294967295.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 4294967295.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;


        case DataPointConfigAccess::DPT_SIGNED_INT32_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset( true, true );
            m_dataPointValueNumberEdit.SetRange( -2147483648.0, 2147483647.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483648.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT32_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset( true, true );
            m_dataPointValueNumberEdit.SetRange( -2147483647.0, 2147483647.0 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT/* &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483647.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
            m_dataPointValueNumberEdit.Reset( false, true );
            m_dataPointValueNumberEdit.SetRange( -3.4E+38, 3.4E+38 );
            if( m_dataPointValueType == NUMERIC_DATA_POINT /*&&
                m_dataPointValue.numericValue().value >= -3.4E+38 &&
                m_dataPointValue.numericValue().value <= 3.4E+38*/ )
            {
                m_dataPointValueNumberEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueNumberEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_TEXT:
            m_dataPointValueTextEdit.LimitText( TA_Base_Bus::DPT_MAX_TEXT_LENGTH );
            m_dataPointValueTextEdit.SetReadOnly( FALSE );
            if( m_dataPointValueType == TEXT_DATA_POINT &&
                strcmp( strDatapointNewValue.c_str(), UNDEFINED_DATA_POINT_VALUE ) )
            {
                m_dataPointValueTextEdit.SetWindowText( strDatapointNewValue.c_str() );
            }
            m_dataPointValueTextEdit.ShowWindow( SW_SHOW );
            break;

        case DataPointConfigAccess::DPT_BOOLEAN:
        case DataPointConfigAccess::DPT_COMBINED_OUT:
        {
            std::string dataPointName = strDatapointName;
            DataPointConfigAccess::StateValueList derivedStates = DataPointConfigAccess::getDataPointStates( dataPointName );

            m_dataPointValueSetCombo.ResetContent();
            for( DataPointConfigAccess::StateValueList::iterator iter = derivedStates.begin(); iter != derivedStates.end(); iter++ )
            {
                DataPointConfigAccess::StateValue derivedState( *iter );
                std::ostringstream stateLabel;
                stateLabel << derivedState.state << " : " << derivedState.description;

                int stateIdx = m_dataPointValueSetCombo.AddString( stateLabel.str().c_str() );
                m_dataPointValueSetCombo.SetItemData( stateIdx, derivedState.state );

                int nNewValue = atoi( strDatapointNewValue.c_str() );
                if( ( m_dataPointValueType == COMBINED_DATA_POINT || m_dataPointValueType == BOOLEAN_DATA_POINT ) && ( nNewValue == derivedState.state ) )
                {
                    m_dataPointValueSetCombo.SetCurSel( stateIdx );
                }
            }
            m_dataPointValueSetCombo.ShowWindow( SW_SHOW );
            break;
        }

        case DataPointConfigAccess::DPT_DERIVED_IN:
            m_dataPointValueTextEdit.SetWindowText( READONLY_DATA_POINT );
            m_dataPointValueTextEdit.SetReadOnly( TRUE );
            m_dataPointValueTextEdit.ShowWindow( SW_SHOW );
            break;

        default:
            m_dataPointValueTextEdit.SetWindowText( UNDEFINED_DATA_POINT_VALUE );
            m_dataPointValueTextEdit.SetReadOnly( TRUE );
            m_dataPointValueTextEdit.ShowWindow( SW_SHOW );
    }

    UpdateData( FALSE );

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointSetPage::UpdateDataPointValue()
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
            updateValue( ParamNewValue, m_dataPointValueNumberEdit );
            break;
        }

        case DataPointConfigAccess::DPT_BOOLEAN:
        {
            updateValue( ParamNewValue, m_dataPointValueSetCombo );
            break;
        }

        case DataPointConfigAccess::DPT_TEXT:
        {
            updateValue( ParamNewValue, m_dataPointValueTextEdit );;
            break;
        }

        case DataPointConfigAccess::DPT_COMBINED_OUT:
        {
            updateValue( ParamNewValue, m_dataPointValueSetCombo );
            break;
        }

        default:
        {
            updateValue( ParamNewValue, m_dataPointValueTextEdit );;
            break;
        }
    }

    FUNCTION_EXIT;
}

void CPlanStepEditDataPointSetPage::updateValue( const std::string& strName, CWnd& refDispWnd )
{
    if( &refDispWnd == &m_dataPointValueSetCombo )
    {
        int curSel = m_dataPointValueSetCombo.GetCurSel();
        unsigned int key = m_dataPointValueSetCombo.GetItemData( curSel );
        std::vector<unsigned int> vecSelectedKeys;
        vecSelectedKeys.push_back( key );
        std::string strDisplayValue = "";
        GetPlanStep()->getActivityParameter().getListPicker( ParamNewValue )->pickFromList( ParamNewValue, vecSelectedKeys, strDisplayValue );
    }
    else
    {
        CPlanStepEditParameterPage::updateValue( strName, refDispWnd );
    }
}

}
