//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivePlanCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
#define AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class PlanCheckStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditActivePlanCheckPage dialog

    class CPlanStepEditActivePlanCheckPage : public CPlanStepEditParameterPage
    {
    // Construction
    public:
        CPlanStepEditActivePlanCheckPage(PlanCheckStep* parameter);   // standard constructor


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditActivePlanCheckPage)
        CEdit    m_noStepLabel;
        CEdit    m_yesStepLabel;
        CEdit    m_planPathLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditActivePlanCheckPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditActivePlanCheckPage)
        afx_msg void OnPickNoStepButton();
        afx_msg void OnPickYesStepButton();
        afx_msg void OnPickPlanPathButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // !defined(AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
