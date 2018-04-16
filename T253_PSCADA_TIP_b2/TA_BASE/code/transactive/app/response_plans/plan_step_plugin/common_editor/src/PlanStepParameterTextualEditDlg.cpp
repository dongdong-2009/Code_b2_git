//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterTextualEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "core/utilities/src/DebugUtil.h"
#include "Resource.h"
#include "PlanStepParameterTextualEditDlg.h"
#include "TextualStepParameter.h"


namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameterTextualEditDlg dialog

CPlanStepParameterTextualEditDlg::CPlanStepParameterTextualEditDlg(TextualStepParameter& parameter,bool canEdit)
: CPlanStepParameterEditDlg( parameter, IDD_PLAN_STEP_PARAMETER_TEXTUAL_EDIT_DLG, canEdit )
{
    FUNCTION_ENTRY( "CPlanStepParameterTextualEditDlg" );
    FUNCTION_EXIT;
}


void CPlanStepParameterTextualEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterTextualEditDlg)
    DDX_Control(pDX, IDC_PARAMETER_VALUE, m_textValue);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterTextualEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterTextualEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTextualEditDlg message handlers


BOOL CPlanStepParameterTextualEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    TextualStepParameter* textualParameter = dynamic_cast<TextualStepParameter*>(GetStepParameter());
    TA_ASSERT(textualParameter!=NULL,"Not a textual step parameter");

    m_textValue.SetWindowText(textualParameter->value().c_str());

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterTextualEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    CString text;
    m_textValue.GetWindowText(text);

    TextualStepParameter* textualParameter = dynamic_cast<TextualStepParameter*>(GetStepParameter());
    TA_ASSERT(textualParameter!=NULL,"Not a textual step parameter");
    textualParameter->value(text.operator LPCSTR());

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}

}