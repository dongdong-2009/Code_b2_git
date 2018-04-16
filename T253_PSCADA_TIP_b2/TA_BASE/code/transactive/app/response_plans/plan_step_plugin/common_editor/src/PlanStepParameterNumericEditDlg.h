//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterNumericEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_)
#define AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_

#include "PlanStepParameterEditDlg.h"

namespace TA_Base_App
{
    class NumericStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterNumericEditDlg dialog

    class CPlanStepParameterNumericEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterNumericEditDlg(NumericStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterNumericEditDlg)
        CString    m_units;
        CString    m_value;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterNumericEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterNumericEditDlg)
        virtual afx_msg void onOK();
        virtual afx_msg BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // End namespace

#endif // !defined(AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_)
