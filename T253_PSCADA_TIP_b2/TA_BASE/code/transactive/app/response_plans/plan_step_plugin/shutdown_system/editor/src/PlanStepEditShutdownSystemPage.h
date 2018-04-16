//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditShutdownSystemPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>
#if !defined(AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_)
#define AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"


namespace TA_Base_App
{
    class ShutdownSystemStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditShutdownSystemPage dialog

    class CPlanStepEditShutdownSystemPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditShutdownSystemPage)

        // Construction
    public:
        CPlanStepEditShutdownSystemPage(ShutdownSystemStep* step);
        ~CPlanStepEditShutdownSystemPage();


        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditShutdownSystemPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditShutdownSystemPage)
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditShutdownSystemPage)
        CButton    m_nofityPeer;
        // }}AFX_DATA
    };

} // end namespace

#endif // !defined(AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_)
