//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditFailureSummaryParameterPage.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>


#if !defined(AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_)
#define AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanStepEditFailureSummaryParameterPage.h : header file
//
#include <afxcmn.h>
#include <afxdtctl.h>

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"


namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditFailureSummaryParameterPage dialog
    class FailureSummaryStep;

    class CPlanStepEditFailureSummaryParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditFailureSummaryParameterPage)

        // Construction
    public:
        CPlanStepEditFailureSummaryParameterPage(FailureSummaryStep* step);
        ~CPlanStepEditFailureSummaryParameterPage() {}

        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // Dialog Data
        // {{AFX_DATA(CPlanStepEditFailureSummaryParameterPage)
		CButton      m_failureAllSteps;
		CButton      m_failureSetDataPointSteps;
    // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditFailureSummaryParameterPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditFailureSummaryParameterPage)
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    //private:
    //    enum { UNDEFINED_STEPS = -1,
    //           ALL_STEPS = 0,
    //           BRC_STEPS_ONLY = 1};
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_)
