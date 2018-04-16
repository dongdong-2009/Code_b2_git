//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDecisionParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
#define AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_

#include <afxcmn.h>
#include <afxdtctl.h>

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"


namespace TA_Base_App
{
    class DecisionStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDecisionParameterPage dialog

    class CPlanStepEditDecisionParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDecisionParameterPage)

    // Construction
    public:
        CPlanStepEditDecisionParameterPage(DecisionStep* parameter);
        ~CPlanStepEditDecisionParameterPage();

    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDecisionParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDecisionParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnNoPickStepButton();
        afx_msg void OnYesPickStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDecisionParameterPage)
        CEdit m_decisionMessage;
        CDateTimeCtrl m_responseTimeout;
        CEdit m_yesStep;
        CEdit m_noStep;
        CEdit m_yesBtnLabel;
        CEdit m_noBtnLabel;
        CEdit m_pauseBtnLabel;
        CButton m_noBtnEnabled;
        CButton m_pauseBtnEnabled;
        // }}AFX_DATA
    };
}

#endif // !defined(AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
