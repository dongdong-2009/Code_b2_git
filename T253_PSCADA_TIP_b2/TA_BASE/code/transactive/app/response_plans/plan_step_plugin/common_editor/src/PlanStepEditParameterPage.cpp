//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepEditParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/TextEditDialog.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/GridEditorDialog.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/ListPickerDialog.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/TreePickerDialog.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/Resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ApplicationFont.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditParameterPage, CPropertyPage)

CPlanStepEditParameterPage::CPlanStepEditParameterPage(UINT dialogIDD, PlanStep* step)
   : CPropertyPage(dialogIDD), m_step(step), m_canEdit(false)
{
    FUNCTION_ENTRY( "CPlanStepEditParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage::CPlanStepEditParameterPage(PlanStep* step)
: CPropertyPage( IDD_PLAN_STEP_EDIT_PARAMETERS_DLG ), m_step( step ), m_canEdit( false )
{
    FUNCTION_ENTRY( "CPlanStepEditParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage::~CPlanStepEditParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditParameterPage" );
    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}

BEGIN_MESSAGE_MAP(CPlanStepEditParameterPage, CPropertyPage)
    // {{AFX_MSG_MAP(CPlanStepEditParameterPage)
    ON_MESSAGE_VOID(WM_DISPLAY_STEP_PARAMETERS, OnDisplayParameters)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage message handlers

BOOL CPlanStepEditParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPropertyPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    PostMessage(WM_DISPLAY_STEP_PARAMETERS, 0, 0); // Call DisplayParameters() after OnInitDialog() has returned

    FUNCTION_EXIT;
    return TRUE;
}

void CPlanStepEditParameterPage::OnDisplayParameters()
{
    FUNCTION_ENTRY( "OnDisplayParameters" );

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() ); //TODO

    EnableParameterControls(m_canEdit);  // TD#614/1170/1409
    DisplayParameters();

    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::SaveParameters()
{
    FUNCTION_ENTRY( "SaveParameters" );

    // Check if the dialog is displayed as this is a property page
    if (GetSafeHwnd()!=NULL)
    {
        UpdateData();
        UpdateParameters();
    }

    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::EnableParameterControls(bool /* enable */)
{
    FUNCTION_ENTRY( "EnableParameterControls" );
    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    FUNCTION_EXIT;
}

void CPlanStepEditParameterPage::valueChanged( IActivityParameter& refParameter, const std::string& strName, const std::string& strDisplayValue )
{
    bool bValid = false;
    IStringEditor* pStringEditor = refParameter.getStringEditor( strName );

    if ( NULL != pStringEditor )
    {
        pStringEditor->updateValue( strName, strDisplayValue, bValid );
    }
}

void CPlanStepEditParameterPage::booleanValueChanged( IActivityParameter& refParameter, const std::string& strName, const bool bNewValue )
{
    bool bValid = false;
    IBoolEditor* pBoolEditor = refParameter.getBoolEditor( strName );

    if ( NULL != pBoolEditor )
    {
        std::string strDisplayValue = "";
        pBoolEditor->updateBooleanValue( strName, bNewValue, strDisplayValue, bValid );
    }
}

void CPlanStepEditParameterPage::timeValueChanged( IActivityParameter& refParameter, const std::string& strName, const time_t tmNewValue )
{
    bool bValid = false;
    ITimeEditor* pTimeEditor = refParameter.getTimeEditor( strName );

    if ( NULL != pTimeEditor )
    {
        std::string strDisplayValue = "";
        pTimeEditor->updateTimeValue( strName, tmNewValue, strDisplayValue, bValid );
    }
}

void CPlanStepEditParameterPage::getBooleanValue( IActivityParameter& refParameter, const std::string& strName, bool& bCurValue )
{
    IBoolEditor* pBoolEditor = refParameter.getBoolEditor( strName );

    if ( NULL != pBoolEditor )
    {
        pBoolEditor->getBooleanValue( strName, bCurValue );
    }
}

void CPlanStepEditParameterPage::popupList( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue )
{
    IListPicker* pListPicker = refParameter.getListPicker( strName );

    if ( NULL == pListPicker )
    {
        return;
    }

    ListPickerDialog dlgPopup( *pListPicker, strName, this );

    dlgPopup.Title = strTitle.c_str();

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        strDisplayValue = dlgPopup.DisplayValue.c_str();
    }
}

void CPlanStepEditParameterPage::popupTree( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue )
{
    ITreePicker* pTreePicker = refParameter.getTreePicker( strName );

    if ( NULL == pTreePicker )
    {
        return;
    }

    TreePickerDialog dlgPopup( *pTreePicker, strName, this );

    dlgPopup.Title = strTitle.c_str();

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        strDisplayValue = dlgPopup.DisplayValue.c_str();
    }
}

void CPlanStepEditParameterPage::popupGrid( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue )
{
    IGridEditor* pGridEditor = refParameter.getGridEditor( strName );

    if ( NULL == pGridEditor )
    {
        return;
    }

    GridEditorDialog dlgPopup( *pGridEditor, strName, this );

    dlgPopup.Title = strTitle.c_str();

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        strDisplayValue = dlgPopup.DisplayValue.c_str();
    }
}

void CPlanStepEditParameterPage::popupText( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue )
{
    TextEditDialog dlgPopup( this );

    dlgPopup.Title = strTitle.c_str();
    dlgPopup.Content = strDisplayValue.c_str();

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        bool bValid = false;

        IStringEditor* pStringEditor = refParameter.getStringEditor( strName );

        if ( NULL != pStringEditor )
        {
            pStringEditor->updateValue( strName, dlgPopup.Content, bValid );
        }

        strDisplayValue = dlgPopup.Content.c_str();
    }
}

void CPlanStepEditParameterPage::popupList( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd )
{
    CString strValue = "";
    refDispWnd.GetWindowText( strValue );

    std::string strDisplayValue = strValue.operator LPCSTR();
    popupList( GetPlanStep()->getActivityParameter(), strName, strTitle, strDisplayValue );

    if ( 0 != strDisplayValue.compare( strValue.operator LPCSTR() ))
    {
        refDispWnd.SetWindowText( strDisplayValue.c_str() );
    }
}

void CPlanStepEditParameterPage::popupTree( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd )
{
    CString strValue = "";
    refDispWnd.GetWindowText( strValue );

    std::string strDisplayValue = strValue.operator LPCSTR();
    popupTree( GetPlanStep()->getActivityParameter(), strName, strTitle, strDisplayValue );

    if ( 0 != strDisplayValue.compare( strValue.operator LPCSTR() ) )
    {
        refDispWnd.SetWindowText( strDisplayValue.c_str() );
    }
}

void CPlanStepEditParameterPage::popupText( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd )
{
    CString strValue = "";
    refDispWnd.GetWindowText( strValue );

    std::string strDisplayValue = strValue.operator LPCSTR();
    popupText( GetPlanStep()->getActivityParameter(), strName, strTitle, strDisplayValue );

    if ( 0 != strDisplayValue.compare( strValue.operator LPCSTR() ) )
    {
        refDispWnd.SetWindowText( strDisplayValue.c_str() );
    }
}

void CPlanStepEditParameterPage::popupGrid( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd )
{
    CString strValue = "";
    refDispWnd.GetWindowText( strValue );

    std::string strDisplayValue = strValue.operator LPCSTR();
    popupText( GetPlanStep()->getActivityParameter(), strName, strTitle, strDisplayValue );

    if ( 0 != strDisplayValue.compare( strValue.operator LPCSTR() ))
    {
        refDispWnd.SetWindowText( strDisplayValue.c_str() );
    }
}

void CPlanStepEditParameterPage::updateValue( const std::string& strName, CWnd& refDispWnd )
{
    CString strValue = "";
    refDispWnd.GetWindowText( strValue );

    std::string strDisplayValue = strValue.operator LPCSTR();
    valueChanged( GetPlanStep()->getActivityParameter(), strName, strDisplayValue );

    if ( 0 != strDisplayValue.compare( strValue.operator LPCSTR() ))
    {
        refDispWnd.SetWindowText( strDisplayValue.c_str() );
    }
}

void CPlanStepEditParameterPage::fillCheckbox( const std::string& strName, CButton& refDispWnd )
{
    bool bValue = false;

    GetPlanStep()->getActivityParameter().getBoolEditor( strName )->getBooleanValue( strName, bValue );

    refDispWnd.SetCheck( bValue ? BST_CHECKED : BST_UNCHECKED );
}

void CPlanStepEditParameterPage::updateBooleanValue( const std::string& strName, CButton& refDispWnd )
{
    booleanValueChanged( GetPlanStep()->getActivityParameter(), strName, (BST_CHECKED == refDispWnd.GetCheck()) );
}

void CPlanStepEditParameterPage::updateTimeValue( const std::string& strName, CDateTimeCtrl& refDispWnd )
{
    CTime ctmTime;
    refDispWnd.GetTime( ctmTime );
    const time_t tmValue = ctmTime.GetTime();
    std::string strDisplayerValue = "";
    bool bValid = false;

    GetPlanStep()->getActivityParameter().getTimeEditor( strName )->updateTimeValue( strName, tmValue, strDisplayerValue, bValid );
}

void CPlanStepEditParameterPage::fillTimePicker( const std::string& strName, CDateTimeCtrl& refDispWnd )
{
    time_t tmValue = 0u;

    GetPlanStep()->getActivityParameter().getTimeEditor( strName )->getTimeValue( strName, tmValue );

    CTime ctmTime( tmValue );
    refDispWnd.SetTime( &ctmTime );
}

}
