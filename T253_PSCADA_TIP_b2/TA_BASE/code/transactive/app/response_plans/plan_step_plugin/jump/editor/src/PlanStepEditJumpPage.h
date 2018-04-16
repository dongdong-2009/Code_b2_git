//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditJumpPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANSTEPEDITJUMPPAGE_H__6E370B30_B840_4F8B_857A_D63E7EFA010A__INCLUDED_)
#define AFX_PLANSTEPEDITJUMPPAGE_H__6E370B30_B840_4F8B_857A_D63E7EFA010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class JumpStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditJumpPage dialog

    class CPlanStepEditJumpPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditJumpPage)

    // Construction
    public:
        CPlanStepEditJumpPage(JumpStep* step);
        ~CPlanStepEditJumpPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditJumpPage)
        CEdit    m_jumpStep;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditJumpPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditJumpPage)
        afx_msg void OnPickStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace

#endif // !defined(AFX_PLANSTEPEDITJUMPPAGE_H__6E370B30_B840_4F8B_857A_D63E7EFA010A__INCLUDED_)
