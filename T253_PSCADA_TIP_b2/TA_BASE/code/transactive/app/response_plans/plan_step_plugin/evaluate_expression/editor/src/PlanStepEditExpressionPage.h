//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditExpressionParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Evaluate Expression step.

#if !defined(AFX_PLANSTEPEDITExpressionPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
#define AFX_PLANSTEPEDITExpressionPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class ExpressionStep;
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditExpressionPage dialog
    class CPlanStepEditExpressionPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditExpressionPage)

    // Construction
    public:
        CPlanStepEditExpressionPage(ExpressionStep* step);

        ~CPlanStepEditExpressionPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditExpressionPage)
        CEdit    m_expression;
        CEdit    m_failStepLabel;
        CEdit    m_successStepLabel;
    // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditExpressionPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
         virtual BOOL OnInitDialog ();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditExpressionPage)
        afx_msg void OnPickFailStepButton();
        afx_msg void OnPickSuccessStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITExpressionPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
