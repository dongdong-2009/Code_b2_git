//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTerminatePlanPage.h $
// @author:  Andy Parker
// @version: $Revision: #6 $
//
// Last modification: $DateTime: 2015/01/22 11:33:34 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
#define AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class TerminatePlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditTerminatePlanPage dialog

    class CPlanStepEditTerminatePlanPage : public CPlanStepEditParameterPage
    {
    // Construction
    public:
        CPlanStepEditTerminatePlanPage(TerminatePlanStep* parameter);   // standard constructor


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditTerminatePlanPage)
        CEdit    m_planPathLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditTerminatePlanPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditTerminatePlanPage)
        afx_msg void OnPickPlanPathButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // !defined(AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
