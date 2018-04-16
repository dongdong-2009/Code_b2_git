//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterYesNoEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "core/utilities/src/DebugUtil.h"
#include "Resource.h"
#include "PlanStepParameterYesNoEditDlg.h"
#include "YesNoStepParameter.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterYesNoEditDlg dialog

CPlanStepParameterYesNoEditDlg::CPlanStepParameterYesNoEditDlg(YesNoStepParameter& parameter,bool canEdit)
: CPlanStepParameterEditDlg( parameter, IDD_PLAN_STEP_PARAMETER_YES_NO_EDIT_DLG, canEdit )
{
    FUNCTION_ENTRY( "CPlanStepParameterYesNoEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterYesNoEditDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterYesNoEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterYesNoEditDlg)
    DDX_Control(pDX, IDC_PARAMETER_VALUE, m_valueCheck);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterYesNoEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterYesNoEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterYesNoEditDlg message handlers

void CPlanStepParameterYesNoEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    YesNoStepParameter* yesnoParameter = dynamic_cast<YesNoStepParameter*>(GetStepParameter());
    TA_ASSERT(yesnoParameter!=NULL,"Not a yes/no step parameter");

    yesnoParameter->value( m_valueCheck.GetCheck() == BST_CHECKED );

    EndDialog(IDOK);

    FUNCTION_EXIT;
}


BOOL CPlanStepParameterYesNoEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    YesNoStepParameter* yesnoParameter = dynamic_cast<YesNoStepParameter*>(GetStepParameter());
    TA_ASSERT(yesnoParameter!=NULL,"Not a yes/no step parameter");

    m_valueCheck.SetWindowText(yesnoParameter->name().c_str());
    m_valueCheck.SetCheck( yesnoParameter->value() ? BST_CHECKED : BST_UNCHECKED );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

}
