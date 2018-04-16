//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterYesNoEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_)
#define AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_

#include "PlanStepParameterEditDlg.h"


namespace TA_Base_App
{
    class YesNoStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterYesNoEditDlg dialog

    class CPlanStepParameterYesNoEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterYesNoEditDlg(YesNoStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterYesNoEditDlg)
        CButton    m_valueCheck;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterYesNoEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterYesNoEditDlg)
        afx_msg void onOK();
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // namespace

#endif // !defined(AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_)
