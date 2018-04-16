//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterStepNumberEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_)
#define AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_

#include "StdAfx.h"

#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterStepNumberEditDlg dialog

    class CPlanStepParameterStepNumberEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterStepNumberEditDlg(StepNumberStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterStepNumberEditDlg)
        //CPlanStepListCtrl    m_stepList;--lixiaoxiaTODO
		CEditableListCtrl m_stepList;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterStepNumberEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterStepNumberEditDlg)
        virtual afx_msg BOOL OnInitDialog();
        virtual afx_msg void onOK();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		void SetSelectedStep(PlanStep *planStep, bool scrollIntoView = false);
		int FindStep(const unsigned long stepNumber);
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // End namespace
#endif // !defined(AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_)
