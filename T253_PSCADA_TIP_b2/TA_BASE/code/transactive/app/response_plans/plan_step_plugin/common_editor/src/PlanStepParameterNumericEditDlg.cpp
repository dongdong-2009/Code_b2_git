//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterNumericEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "Resource.h"
#include "NumericStepParameter.h"
#include "PlanStepParameterNumericEditDlg.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterNumericEditDlg dialog

CPlanStepParameterNumericEditDlg::CPlanStepParameterNumericEditDlg(NumericStepParameter& parameter,bool canEdit)
: CPlanStepParameterEditDlg( parameter, IDD_PLAN_STEP_NUMERIC_PARAMETER_EDIT_DLG, canEdit )
{
    FUNCTION_ENTRY( "CPlanStepParameterNumericEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterNumericEditDlg)
    m_units = _T("");
    m_value = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterNumericEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterNumericEditDlg)
    DDX_Text(pDX, ID_VALUE_UNITS, m_units);
    DDX_Text(pDX, IDC_PARAMETER_VALUE, m_value);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterNumericEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterNumericEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterNumericEditDlg message handlers

//DEL void CPlanStepParameterNumericEditDlg::setUnitLabel(const char *unitName)
//DEL {
//DEL     m_units = unitName;
//DEL }

void CPlanStepParameterNumericEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    UpdateData(true);
    NumericStepParameter* numericParameter = dynamic_cast<NumericStepParameter*>(GetStepParameter());
    TA_ASSERT(numericParameter!=NULL,"Not a numeric step parameter");
    numericParameter->value(atol(m_value));

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}

BOOL CPlanStepParameterNumericEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    NumericStepParameter* numericParameter = dynamic_cast<NumericStepParameter*>(GetStepParameter());
    TA_ASSERT(numericParameter!=NULL,"Not a numeric step parameter");

    m_value = TA_Base_Core::DataConversion::toString(numericParameter->value()).c_str();

    FUNCTION_EXIT;
    return CPlanStepParameterEditDlg::OnInitDialog();
}

}