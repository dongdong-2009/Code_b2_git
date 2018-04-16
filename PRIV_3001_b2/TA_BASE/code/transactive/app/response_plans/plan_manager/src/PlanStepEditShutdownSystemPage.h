//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditShutdownSystemPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>
#if !defined(AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_)
#define AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_SHUTDOWN_SYSTEM)

#include "PlanStepEditParameterPage.h"


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
        enum { IDD = IDD_PLAN_STEP_EDIT_SHUTDOWN_SYSTEM_DLG };

        BOOL    m_nofityPeer;
        // }}AFX_DATA
    };

} // end namespace

#endif // defined (STEPTYPE_SHUTDOWN_SYSTEM)

#endif // !defined(AFX_PLANSTEPEDITLOGPAGE_H__D9FFD6D4_E94A_4e07_948D_60FFF7B7955A__INCLUDED_)
