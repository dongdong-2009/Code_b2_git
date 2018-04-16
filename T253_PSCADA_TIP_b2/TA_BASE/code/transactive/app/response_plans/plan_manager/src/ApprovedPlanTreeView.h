//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_)
#define AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeView.h"

#include "core/threads/src/Thread.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"

namespace TA_Base_App
{
    class CApprovedPlanDoc;
    class ControlPlanThread;

        /////////////////////////////////////////////////////////////////////////////
    // CApprovedPlanTreeView view
    class CApprovedPlanTreeView : public CPlanTreeView
    {
    protected:
        CApprovedPlanTreeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CApprovedPlanTreeView)

    // Operations
    public:
        CApprovedPlanDoc* GetDocument();

    protected:
        virtual void ProcessRparamExecPlanIdUpdate();
        virtual void ProcessRparamAssocAlarmIdUpdate();
        virtual void ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate);
        virtual void ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CApprovedPlanTreeView)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CApprovedPlanTreeView();

        // Generated message map functions
    protected:
        // {{AFX_MSG(CApprovedPlanTreeView)
        afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

	private:
		std::vector<ControlPlanThread*> m_vecControlPlan;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_)
