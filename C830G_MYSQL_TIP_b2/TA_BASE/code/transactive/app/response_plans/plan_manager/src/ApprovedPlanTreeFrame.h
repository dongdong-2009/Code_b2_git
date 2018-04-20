//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_APPROVEDPLANTREEFRAME_H__8C58EB5A_4E66_4812_BDE4_16F6B14B7681__INCLUDED_)
#define AFX_APPROVEDPLANTREEFRAME_H__8C58EB5A_4E66_4812_BDE4_16F6B14B7681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanTreeFrame.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CApprovedPlanTreeFrame frame

    class CApprovedPlanTreeFrame : public CPlanTreeFrame
    {
        DECLARE_DYNCREATE(CApprovedPlanTreeFrame)
    protected:
        CApprovedPlanTreeFrame();           // protected constructor used by dynamic creation

    // Operations
    protected:
        virtual CRuntimeClass *GetLeftPaneRuntimeClass();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CApprovedPlanTreeFrame)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CApprovedPlanTreeFrame();

        // Generated message map functions
        // {{AFX_MSG(CApprovedPlanTreeFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_APPROVEDPLANTREEFRAME_H__8C58EB5A_4E66_4812_BDE4_16F6B14B7681__INCLUDED_)
