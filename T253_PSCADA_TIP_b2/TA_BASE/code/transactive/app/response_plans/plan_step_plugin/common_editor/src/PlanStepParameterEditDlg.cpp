//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterEditDlg.cpp $
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
#include "Resource.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepParameter.h"
#include "ApplicationFont.h"

namespace TA_Base_App
{

const unsigned int WM_ADJUST_FONT = WM_USER + 0x1001;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterEditDlg dialog

CPlanStepParameterEditDlg::CPlanStepParameterEditDlg( IPlanStepParameter& parameter, bool canEdit )
    : CDialog(IDD_PLAN_STEP_PARAMETER_EDIT_DLG, NULL), m_canEdit(canEdit),m_parameter(parameter)
{
    FUNCTION_ENTRY( "CPlanStepParameterEditDlg" );

    m_parameterName = parameter.name().c_str();

    FUNCTION_EXIT;
}


// Called by derived classes to specify dialog template
CPlanStepParameterEditDlg::CPlanStepParameterEditDlg( IPlanStepParameter& parameter, UINT templateID, bool canEdit )
    : CDialog(templateID, NULL), m_canEdit(canEdit),m_parameter(parameter)
{
    FUNCTION_ENTRY( "CPlanStepParameterEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterEditDlg)
    m_parameterName = parameter.name().c_str();
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterEditDlg)
	DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterEditDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanStepParameterEditDlg)
    ON_MESSAGE(WM_ADJUST_FONT, OnAdjustFont)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterEditDlg message handlers

BOOL CPlanStepParameterEditDlg::OnInitDialog()
{
    PostMessage(WM_ADJUST_FONT, 0, 0);

    return CDialog::OnInitDialog();
}

LRESULT CPlanStepParameterEditDlg::OnAdjustFont( WPARAM wParam, LPARAM lParam )
{
    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    return 0u;
}

void CPlanStepParameterEditDlg::present()
{
    if ( NULL == GetSafeHwnd() )
    {
        DoModal();
    }
    else
    {
        ShowWindow( SW_SHOW );
    }
}

void CPlanStepParameterEditDlg::close()
{
    CloseWindow();
}

}