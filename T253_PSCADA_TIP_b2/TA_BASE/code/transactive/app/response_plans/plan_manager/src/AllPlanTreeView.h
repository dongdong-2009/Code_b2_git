//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_)
#define AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeView.h"


namespace TA_Base_App
{
    class CAllPlanDoc;
    class ControlPlanThread;


    /////////////////////////////////////////////////////////////////////////////
    // CAllPlanTreeView view

    class CAllPlanTreeView : public CPlanTreeView
    {
    protected:
        CAllPlanTreeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CAllPlanTreeView)

    // Operations
    public:
        CAllPlanDoc* GetDocument();

    protected:
        virtual void ProcessRparamViewPlanIdUpdate();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CAllPlanTreeView)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CAllPlanTreeView();
        // Generated message map functions
    protected:
        // {{AFX_MSG(CAllPlanTreeView)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        std::vector<ControlPlanThread*> m_vecControlPlan;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_)
