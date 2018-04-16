//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanControllerThread.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>

#if !defined(AFX_PLANCONTROLLERTHREAD_H__B9C9ED6A_7C31_4865_A038_30F75A97665C__INCLUDED_)
#define AFX_PLANCONTROLLERTHREAD_H__B9C9ED6A_7C31_4865_A038_30F75A97665C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FloatingWindowThread.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanControllerThread thread

    class CPlanControllerThread : public CFloatingWindowThread
    {
        DECLARE_DYNAMIC(CPlanControllerThread)

        CPlanControllerThread(TreeNodeId nodeId);
        virtual ~CPlanControllerThread();

    // Operations
    protected:
        // CFloatingWindowThread overrides
        virtual CDocTemplate *GetDocumentTemplate() const;

    // Attributes
    private:

    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanControllerThread)
        // }}AFX_VIRTUAL

    // Implementation
        // Generated message map functions
        // {{AFX_MSG(CPlanControllerThread)
        afx_msg void OnInteractiveStepExecRequest(WPARAM wParam, LPARAM lParam);
        afx_msg void OnPlanStepFailed(WPARAM wParam, LPARAM lParam);
        afx_msg void OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam);
        afx_msg void OnControlledPlanApprovalStateChanged(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANCONTROLLERTHREAD_H__B9C9ED6A_7C31_4865_A038_30F75A97665C__INCLUDED_)
