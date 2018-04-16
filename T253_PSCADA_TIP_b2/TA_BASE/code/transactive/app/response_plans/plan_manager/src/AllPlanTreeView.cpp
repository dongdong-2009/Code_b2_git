//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanTreeView.cpp $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "PlanManager.h"
#include "AllPlanTreeView.h"
#include "AllPlanDoc.h"
#include "TreeNodeFactory.h"
#include "PlanNode.h"
#include "ControlPlanThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeView

IMPLEMENT_DYNCREATE(CAllPlanTreeView, CPlanTreeView)

CAllPlanTreeView::CAllPlanTreeView()
{
    FUNCTION_ENTRY( "CAllPlanTreeView" );
    FUNCTION_EXIT;
}


CAllPlanTreeView::~CAllPlanTreeView()
{
    FUNCTION_ENTRY( "~CAllPlanTreeView" );

    for (std::vector<ControlPlanThread*>::iterator itLoop = m_vecControlPlan.begin(); m_vecControlPlan.end() != itLoop; ++itLoop)
	{
        delete *itLoop;
        *itLoop = NULL;
	}
    m_vecControlPlan.clear();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CAllPlanTreeView, CPlanTreeView)
    // {{AFX_MSG_MAP(CAllPlanTreeView)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


CAllPlanDoc* CAllPlanTreeView::GetDocument()
{
    FUNCTION_ENTRY( "GetDocument" );

    FUNCTION_EXIT;
    return (CAllPlanDoc*)CPlanTreeView::GetDocument();
}

void CAllPlanTreeView::ProcessRparamViewPlanIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamViewPlanIdUpdate" );

    // Activate the approved plan view and populate the root node if necessary.
    //AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_ALLPLANS, 0), 0);

    // The target plan is in the format of a plan ID or plan path, as per the following:
    // 1. <plan ID>[:<instance #>:<location key>]
    // 2. [/<category 1>/<category N>]/<plan name>
    std::string viewPlan(TA_Base_Core::RunParams::getInstance().get(RPARAM_FWDVIEWPLANID));

    unsigned long planNodeId = -1;
    unsigned long instanceNumber = -1;
    unsigned long location = -1;
    std::string planPath;

    if (viewPlan.find_first_not_of(" :0123456789") == std::string::npos)
    {
        std::istringstream viewPlanStrm(viewPlan);
        char colon;

        viewPlanStrm >> planNodeId >> colon >> instanceNumber >> colon >> location;
    }
    else
    {
        planPath = viewPlan;
        if (location = -1)
        {
            location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation();
        }
    }

    // Verify the plan ID, instance and location key
    if (planPath.empty() && (planNodeId == -1 || instanceNumber == -1 && location != -1 || instanceNumber != -1 && location == -1))
    {
        std::ostringstream errorMessage;
        errorMessage << "Error parsing runtime parameter --view-plan-id." << std::endl
                     << "Invalid plan node ID '" << viewPlan << "'." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << viewPlan;
        userMsg.showMsgBox(IDS_UE_210097);

        FUNCTION_EXIT;
        return;
    }

    // Locate the node in the tree, expanding branches along the way
    HTREEITEM nodeItem = NULL;

    if (planPath.empty())
    {
        nodeItem = FindNode(planNodeId, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node ID '" << planNodeId << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planNodeId;
            userMsg.showMsgBox(IDS_UE_210043);

            return;
        }
    }
    else
    {
        nodeItem = FindNode(planPath, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node '" << planPath << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planPath;
                userMsg.showMsgBox(IDS_UE_210116);

            return;
        }
    }

    // Select the node in the plan tree.
    GetTreeCtrl().SelectItem(nodeItem);

    // Verify the node is in fact a plan node
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(nodeItem));
    PlanNode *planNode = dynamic_cast<PlanNode *>(treeNode);
    if (!planNode)
    {
        std::ostringstream errorMessage;
        errorMessage << "Node ID " << treeNode->getNodeId() << " (" << treeNode->getNodeName() << ") is not a plan type node." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << treeNode->getNodeId() << treeNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210044);

        return;
    }

    try
    {
        CWaitCursor waitCursor;

		ControlPlanThread* pControlPlan = new ControlPlanThread(planNode, location, instanceNumber, viewPlan, false);
		pControlPlan->start();
		m_vecControlPlan.push_back(pControlPlan);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << planNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210045);
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeView message handlers
