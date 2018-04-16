//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditAssignCameraToMonitorPage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
//

#if !defined(AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_)
#define AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class AssignCameraToMonitorStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditAssignCameraToMonitorPage dialog

    class CPlanStepEditAssignCameraToMonitorPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditAssignCameraToMonitorPage)

    // Construction
    public:
        CPlanStepEditAssignCameraToMonitorPage(AssignCameraToMonitorStep* step);
        ~CPlanStepEditAssignCameraToMonitorPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditAssignCameraToMonitorPage)
        CEdit    m_camera;
        CEdit    m_monitor;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditAssignCameraToMonitorPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditAssignCameraToMonitorPage)
        afx_msg void OnPickCameraButton();
        afx_msg void OnPickMonitorButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_)
